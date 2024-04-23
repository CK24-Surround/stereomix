// Fill out your copyright notice in the Description page of Project Settings.


#include "SMGameplayAbility_Smash.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "Characters/SMPlayerCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/SMTeamComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NiagaraSystem.h"
#include "Tiles/SMTile.h"
#include "Utilities/SMCalculateBlueprintLibrary.h"
#include "Utilities/SMCollision.h"
#include "Utilities/SMLog.h"
#include "Utilities/SMTags.h"

USMGameplayAbility_Smash::USMGameplayAbility_Smash()
{
	ActivationOwnedTags = FGameplayTagContainer(SMTags::Character::State::Smashing);
	ActivationRequiredTags = FGameplayTagContainer(SMTags::Character::State::Catch);

	OnSmashFX.Add(ESMTeam::None, nullptr);
	OnSmashFX.Add(ESMTeam::EDM, nullptr);
	OnSmashFX.Add(ESMTeam::FutureBass, nullptr);
}

void USMGameplayAbility_Smash::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	USMAbilitySystemComponent* SourceASC = GetSMAbilitySystemComponentFromActorInfo();
	if (!ensure(SourceASC))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	ASMPlayerCharacter* SourceCharacter = GetSMPlayerCharacterFromActorInfo();
	if (!ensure(SourceCharacter))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	ASMPlayerCharacter* TargetCharacter = SourceCharacter->GetCatchCharacter();
	if (!ensure(TargetCharacter))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	USMAbilitySystemComponent* TargetASC = Cast<USMAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetCharacter));
	if (!ensure(TargetASC))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 착지 델리게이트를 기다립니다.
	SourceCharacter->OnLanded.AddUObject(this, &USMGameplayAbility_Smash::OnSmash);

	// 스매시 애니메이션을 재생합니다.
	UAbilityTask_PlayMontageAndWait* PlayMontageAndWaitTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("SmashMontage"), SmashMontage);
	if (!ensure(PlayMontageAndWaitTask))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	PlayMontageAndWaitTask->ReadyForActivation();

	// 타겟 측에 스매시 당하는 중을 표시하는 태그를 부착합니다.
	if (ActorInfo->IsNetAuthority())
	{
		TargetASC->AddTag(SMTags::Character::State::Smashed);
	}

	CommitAbility(Handle, ActorInfo, ActivationInfo);

	UCharacterMovementComponent* SourceMovement = SourceCharacter->GetCharacterMovement();
	if (ensure(SourceMovement))
	{
		OriginalGravityScale = SourceMovement->GravityScale;
		SourceMovement->GravityScale = SmashGravityScale;
	}

	if (ActorInfo->IsLocallyControlled())
	{
		// 시작점과 목표 지점의 위치를 구합니다.
		const FVector SourceLocation = SourceCharacter->GetActorLocation();
		FVector TargetLocation = SourceCharacter->GetCursorTargetingPoint(true);

		// 사거리를 제한 합니다.
		FVector AlignedSourceZ = SourceLocation;
		AlignedSourceZ.Z = TargetLocation.Z;

		const float Range = 150.0f * SmashRangeByTile;
		if (FVector::DistSquared(AlignedSourceZ, TargetLocation) > FMath::Square(Range))
		{
			const FVector SourceToTargetDirection = (TargetLocation - AlignedSourceZ).GetSafeNormal();

			TargetLocation = AlignedSourceZ + (SourceToTargetDirection * Range);
		}

		// 필요한 위치 데이터들을 타겟 데이터에 저장합니다.
		FGameplayAbilityTargetData_LocationInfo* TargetData = new FGameplayAbilityTargetData_LocationInfo();
		TargetData->SourceLocation.LocationType = EGameplayAbilityTargetingLocationType::LiteralTransform;
		TargetData->TargetLocation.LocationType = EGameplayAbilityTargetingLocationType::LiteralTransform;
		TargetData->SourceLocation.LiteralTransform.SetLocation(SourceLocation);
		TargetData->TargetLocation.LiteralTransform.SetLocation(TargetLocation);

		// 타겟 데이터를 핸들에 담고 서버로 전송합니다.
		FGameplayAbilityTargetDataHandle TargetDatahandle(TargetData);
		SourceASC->CallServerSetReplicatedTargetData(Handle, ActivationInfo.GetActivationPredictionKey(), TargetDatahandle, FGameplayTag(), SourceASC->ScopedPredictionKey);

		// 정점 높이를 기준으로 타겟을 향하는 벨로시티를 구한 후 캐릭터를 도약 시킵니다.
		const FVector LaunchVelocity = USMCalculateBlueprintLibrary::SuggestProjectileVelocity_CustomApexHeight(SourceCharacter, SourceLocation, TargetLocation, ApexHeight, SourceMovement->GetGravityZ());
		SourceCharacter->LaunchCharacter(LaunchVelocity, true, true);
	}

	if (ActorInfo->IsNetAuthority())
	{
		// 서버는 데이터를 수신을 준비합니다. 수신 후 알아서 바인드 해제됩니다.
		FAbilityTargetDataSetDelegate& TargetDataSetDelegate = SourceASC->AbilityTargetDataSetDelegate(Handle, ActivationInfo.GetActivationPredictionKey());
		TargetDataSetDelegate.AddUObject(this, &USMGameplayAbility_Smash::OnReceiveTargetData);
	}
}

void USMGameplayAbility_Smash::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USMGameplayAbility_Smash::OnReceiveTargetData(const FGameplayAbilityTargetDataHandle& GameplayAbilityTargetDataHandle, FGameplayTag GameplayTag)
{
	ASMPlayerCharacter* SourceCharacter = GetSMPlayerCharacterFromActorInfo();
	if (!ensureAlways(SourceCharacter))
	{
		return;
	}

	UCharacterMovementComponent* SourceMovement = SourceCharacter->GetCharacterMovement();
	if (!ensureAlways(SourceMovement))
	{
		return;
	}

	const FGameplayAbilityTargetData* TargetData = GameplayAbilityTargetDataHandle.Get(0);
	const FVector SourceLocation = TargetData->GetOrigin().GetLocation();
	const FVector TargetLocation = TargetData->GetEndPoint();

	const FVector LaunchVelocity = USMCalculateBlueprintLibrary::SuggestProjectileVelocity_CustomApexHeight(SourceCharacter, SourceLocation, TargetLocation, ApexHeight, SourceMovement->GetGravityZ());
	SourceCharacter->LaunchCharacter(LaunchVelocity, true, true);
}

void USMGameplayAbility_Smash::OnSmash()
{
	USMAbilitySystemComponent* SourceASC = GetSMAbilitySystemComponentFromActorInfo();
	if (!ensure(SourceASC))
	{
		return;
	}

	ASMPlayerCharacter* SourceCharacter = GetSMPlayerCharacterFromActorInfo();
	if (!ensure(SourceCharacter))
	{
		return;
	}

	ASMPlayerCharacter* TargetCharacter = SourceCharacter->GetCatchCharacter();
	if (!ensure(TargetCharacter))
	{
		return;
	}

	USMAbilitySystemComponent* TargetASC = Cast<USMAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetCharacter));
	if (!ensure(TargetASC))
	{
		return;
	}

	SourceCharacter->OnLanded.RemoveAll(this);

	// 기존 중력 스케일로 재설정합니다.
	UCharacterMovementComponent* SourceMovement = SourceCharacter->GetCharacterMovement();
	if (ensure(SourceMovement))
	{
		SourceMovement->GravityScale = OriginalGravityScale;
	}

	// 스매시 종료 애니메이션을 재생합니다.
	UAbilityTask_PlayMontageAndWait* PlayMontageAndWaitTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("SmashEndMontage"), SmashMontage, 1.0f, TEXT("End"));
	if (!ensure(PlayMontageAndWaitTask))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	PlayMontageAndWaitTask->OnCompleted.AddDynamic(this, &USMGameplayAbility_Smash::OnCompleted);
	PlayMontageAndWaitTask->ReadyForActivation();

	if (CurrentActorInfo->IsNetAuthority())
	{
		// 타일 트리거 로직
		TileTrigger(TargetCharacter);

		// 스매시 대미지 로직
		SmashSplash();

		// 잡기 풀기 로직
		ReleaseCatch(TargetCharacter);

		// 스매시 당하는 상태를 나타내는 태그를 제거해줍니다.
		TargetASC->RemoveTag(SMTags::Character::State::Smashed);

		// 타겟의 Smashed 어빌리티를 활성화합니다.
		TargetASC->TryActivateAbilitiesByTag(FGameplayTagContainer(SMTags::Ability::Smashed));
	}
}

void USMGameplayAbility_Smash::OnCompleted()
{
	if (CurrentActorInfo->IsNetAuthority())
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}

void USMGameplayAbility_Smash::ReleaseCatch(ASMPlayerCharacter* TargetCharacter)
{
	if (!ensure(TargetCharacter))
	{
		return;
	}

	USMAbilitySystemComponent* TargetASC = Cast<USMAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetCharacter));
	if (!ensure(TargetASC))
	{
		return;
	}

	ASMPlayerCharacter* SourceCharacter = GetSMPlayerCharacterFromActorInfo();
	if (!ensure(SourceCharacter))
	{
		return;
	}

	USMAbilitySystemComponent* SourceASC = GetSMAbilitySystemComponentFromActorInfo();
	if (!ensure(SourceASC))
	{
		return;
	}

	// 디태치합니다.
	TargetCharacter->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	// 콜리전, 움직임을 복구합니다.
	TargetCharacter->SetEnableCollision(true);
	TargetCharacter->SetEnableMovement(true);

	// 회전을 소스의 방향에 맞게 리셋해줍니다.
	const float SourceYaw = SourceCharacter->GetActorRotation().Yaw;
	TargetCharacter->MulticastRPCSetYawRotation(SourceYaw);

	// TODO: 애니메이션 불일치에 따른 임시 위치 조정 코드입니다. 추후 제거되어야합니다.
	TargetCharacter->ServerRPCPreventGroundEmbedding();

	// 다시 서버로부터 위치 보정을 수행하도록 변경합니다.
	UCharacterMovementComponent* TargetMovement = TargetCharacter->GetCharacterMovement();
	if (ensure(TargetMovement))
	{
		TargetMovement->bIgnoreClientMovementErrorChecksAndCorrection = false;
	}

	// 카메라 뷰도 원래대로 되돌려줍니다.
	APlayerController* SourcePlayerController = Cast<APlayerController>(TargetCharacter->GetController());
	if (ensure(SourcePlayerController))
	{
		SourcePlayerController->SetViewTargetWithBlend(TargetCharacter, 1.0f, VTBlend_Cubic);
	}

	// 캐릭터 상태 위젯을 다시 보이게합니다.
	TargetCharacter->SetCharacterStateVisibility(true);

	// 잡기, 잡히기 상태를 나타내는 태그를 제거해줍니다.
	SourceASC->RemoveTag(SMTags::Character::State::Catch);
	TargetASC->RemoveTag(SMTags::Character::State::Caught);

	SourceCharacter->SetCatchCharacter(nullptr);
	TargetCharacter->SetCaughtCharacter(nullptr);
}

void USMGameplayAbility_Smash::TileTrigger(ASMPlayerCharacter* InTargetCharacter)
{
	if (!ensure(InTargetCharacter))
	{
		return;
	}

	ASMPlayerCharacter* SourceCharacter = GetSMPlayerCharacterFromActorInfo();
	if (!ensure(SourceCharacter))
	{
		return;
	}

	FHitResult HitResult;
	const FVector Start = InTargetCharacter->GetActorLocation();
	const FVector End = Start + (-FVector::UpVector * 1000.0f);
	const FCollisionQueryParams Param(SCENE_QUERY_STAT(TileTrace), false);
	const FCollisionShape CollisionShape = FCollisionShape::MakeSphere(25.0f);
	const bool bSuccess = GetWorld()->SweepSingleByChannel(HitResult, Start, End, FQuat::Identity, SMCollisionTraceChannel::TileAction, CollisionShape, Param);
	DrawDebugLine(GetWorld(), Start, End, FColor::Silver, false, 3.0f);

	if (bSuccess)
	{
		ASMTile* Tile = Cast<ASMTile>(HitResult.GetActor());
		if (Tile)
		{
			FVector Center = Tile->GetTileLocation();
			NET_LOG(GetSMPlayerCharacterFromActorInfo(), Log, TEXT("%s타일을 트리거 했습니다."), *Tile->GetName());

			const USMTeamComponent* SourceTeamComponent = SourceCharacter->GetTeamComponent();
			if (ensure(SourceTeamComponent))
			{
				const ESMTeam Team = SourceTeamComponent->GetTeam();
				TileTriggerData.TriggerCount = 0;
				TileTriggerData.TriggerStartLocation = Center;
				TileTriggerData.SourceTeam = Team;
				// 충돌 박스의 크기를 0.0으로 하면 오류가 생길 수 있으니 1.0으로 설정해두었습니다.
				TileTriggerData.Range = 1.0f;
				const UBoxComponent* TileBoxComponent = Tile->GetBoxComponent();
				if (ensure(TileBoxComponent))
				{
					const float TileHorizonSize = TileBoxComponent->GetScaledBoxExtent().X * 2.0;
					TileTriggerData.TileHorizonSize = TileHorizonSize;
				}
				ProcessContinuousTileTrigger();

				FGameplayCueParameters GCParams;
				GCParams.Location = Center;
				GCParams.SourceObject = OnSmashFX[Team];

				USMAbilitySystemComponent* SourceASC = GetSMAbilitySystemComponentFromActorInfo();
				if (ensure(SourceASC))
				{
					SourceASC->ExecuteGameplayCue(SMTags::GameplayCue::PlayNiagara, GCParams);
				}
			}
		}
	}
}

void USMGameplayAbility_Smash::ProcessContinuousTileTrigger()
{
	// 타일 트리거 영역입니다.
	FVector HalfExtend(TileTriggerData.Range, TileTriggerData.Range, 100.0);

	TArray<FOverlapResult> OverlapResults;
	FCollisionShape CollisionShape = FCollisionShape::MakeBox(HalfExtend);
	const bool bSuccess = GetWorld()->OverlapMultiByChannel(OverlapResults, TileTriggerData.TriggerStartLocation, FQuat::Identity, SMCollisionTraceChannel::TileAction, CollisionShape);
	if (bSuccess)
	{
		for (const auto& OverlapResult : OverlapResults)
		{
			ASMTile* Tile = Cast<ASMTile>(OverlapResult.GetActor());
			if (Tile)
			{
				Tile->TileTrigger(TileTriggerData.SourceTeam);
			}
		}
	}

	DrawDebugBox(GetWorld(), TileTriggerData.TriggerStartLocation, HalfExtend, FColor::Turquoise, false, 2.0f);

	// -1을 통해 원하는 횟수만큼 타이머를 동작 시킬 수 있습니다.
	if (TileTriggerData.TriggerCount < MaxTriggerCount - 1)
	{
		// 다음 트리거를 위해 값을 더해줍니다.
		TileTriggerData.Range += TileTriggerData.TileHorizonSize;
		++TileTriggerData.TriggerCount;

		// 아래 수식으로 몇 초 뒤에 다시 트리거되어야하는지 구해서 타이머에 적용합니다.
		FTimerHandle TimerHandle;
		const float TimeAdd = TotalTriggerTime / (MaxTriggerCount - 1);
		// NET_LOG(GetSMPlayerCharacterFromActorInfo(), Warning, TEXT("타일 트리거 시작 TimeAdd: %f"), TimeAdd);
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &USMGameplayAbility_Smash::ProcessContinuousTileTrigger, TimeAdd, false);
	}
}

void USMGameplayAbility_Smash::SmashSplash()
{
	const ASMPlayerCharacter* SourceCharacter = GetSMPlayerCharacterFromActorInfo();
	if (!ensureAlways(SourceCharacter))
	{
		return;
	}

	USMAbilitySystemComponent* SourceASC = GetSMAbilitySystemComponentFromActorInfo();
	if (!ensureAlways(SourceASC))
	{
		return;
	}

	const USMTeamComponent* SourceTeamComponent = SourceCharacter->GetTeamComponent();
	if (!ensureAlways(SourceTeamComponent))
	{
		return;
	}

	const ESMTeam SourceTeam = SourceTeamComponent->GetTeam();
	if (SourceTeam == ESMTeam::None)
	{
		return;
	}

	TArray<FOverlapResult> OverlapResults;
	const FVector Start = SourceCharacter->GetActorLocation();
	const float HorizenSize = (TileTriggerData.TileHorizonSize * (MaxTriggerCount - 1)) + TileTriggerData.TileHorizonSize / 2;
	const FVector CollisionHalfExtend(HorizenSize, HorizenSize, 100.0);
	const FCollisionShape CollisionShape = FCollisionShape::MakeBox(CollisionHalfExtend);
	const FCollisionQueryParams Params(SCENE_QUERY_STAT(SmashSplash), false, SourceCharacter);
	const bool bSuccess = GetWorld()->OverlapMultiByChannel(OverlapResults, Start, FQuat::Identity, SMCollisionTraceChannel::Action, CollisionShape, Params);

	if (bSuccess)
	{
		for (const auto& OverlapResult : OverlapResults)
		{
			ASMPlayerCharacter* TargetCharacter = Cast<ASMPlayerCharacter>(OverlapResult.GetActor());
			if (!TargetCharacter)
			{
				continue;
			}

			USMAbilitySystemComponent* TargetASC = Cast<USMAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetCharacter));
			if (!ensureAlways(TargetASC))
			{
				continue;
			}

			USMTeamComponent* TeamComponent = TargetCharacter->GetTeamComponent();
			if (!ensureAlways(TeamComponent))
			{
				continue;
			}

			const ESMTeam TargetTeam = TeamComponent->GetTeam();
			if (TargetTeam == ESMTeam::None)
			{
				continue;
			}

			if (SourceTeam == TargetTeam)
			{
				continue;
			}

			FGameplayEffectSpecHandle GESpecHandle = MakeOutgoingGameplayEffectSpec(DamageGE);
			FGameplayEffectSpec* GESpec = GESpecHandle.Data.Get();
			if (ensureAlways(GESpec))
			{
				GESpec->SetSetByCallerMagnitude(SMTags::Data::Damage, SmashDamage);
			}

			SourceASC->BP_ApplyGameplayEffectSpecToTarget(GESpecHandle, TargetASC);
		}
	}

	DrawDebugBox(GetWorld(), TileTriggerData.TriggerStartLocation, CollisionHalfExtend, FColor::Orange, false, 2.0f);
}
