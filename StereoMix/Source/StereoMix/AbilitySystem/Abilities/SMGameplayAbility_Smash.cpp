// Copyright Surround, Inc. All Rights Reserved.


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
#include "AbilitySystem/SMTags.h"
#include "Engine/OverlapResult.h"

USMGameplayAbility_Smash::USMGameplayAbility_Smash()
{
	ActivationOwnedTags = FGameplayTagContainer(SMTags::Character::State::Smashing);
	ActivationRequiredTags = FGameplayTagContainer(SMTags::Character::State::Catch);

	OnSmashFX.Add(ESMTeam::None, nullptr);
	OnSmashFX.Add(ESMTeam::EDM, nullptr);
	OnSmashFX.Add(ESMTeam::FutureBass, nullptr);

	SplashDamageFX.Add(ESMTeam::None, nullptr);
	SplashDamageFX.Add(ESMTeam::EDM, nullptr);
	SplashDamageFX.Add(ESMTeam::FutureBass, nullptr);
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
	if (!ensureAlways(InTargetCharacter))
	{
		return;
	}

	ASMPlayerCharacter* SourceCharacter = GetSMPlayerCharacterFromActorInfo();
	if (!ensureAlways(SourceCharacter))
	{
		return;
	}

	USMAbilitySystemComponent* SourceASC = GetSMAbilitySystemComponentFromActorInfo();
	if (!ensureAlways(SourceASC))
	{
		return;
	}

	// 타일을 트리거합니다.
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
			// 트리거된 타일을 타겟 데이터에 담습니다. 타일 트리거 GA로 전송하기 위함입니다.
			FGameplayAbilityTargetData_SingleTargetHit* TagetData = new FGameplayAbilityTargetData_SingleTargetHit();
			TagetData->HitResult = HitResult;

			FGameplayAbilityTargetDataHandle TargetDataHandle;
			TargetDataHandle.Add(TagetData);

			// TargetData에 Tile 액터를, EventMagnitude에 트리거되야하는 규모를 적용합니다. 이 데이터 또한 타일 트리거 GA로 전송하기 위해 저장합니다.
			FGameplayEventData Payload;
			Payload.TargetData = TargetDataHandle;
			Payload.EventMagnitude = MaxTriggerCount;

			// 이펙트 재생에 필요한 데이터들을 GCParams에 담습니다.
			FGameplayCueParameters GCParams;

			FVector TileCenter = Tile->GetTileLocation();
			GCParams.Location = TileCenter;

			const USMTeamComponent* SourceTeamComponent = SourceCharacter->GetTeamComponent();
			if (ensure(SourceTeamComponent))
			{
				const ESMTeam Team = SourceTeamComponent->GetTeam();
				UNiagaraSystem* OnSmashFXWithTeam = OnSmashFX[Team];
				GCParams.SourceObject = OnSmashFXWithTeam;
			}

			// 이펙트를 재생합니다.
			SourceASC->ExecuteGameplayCue(SMTags::GameplayCue::PlayNiagara, GCParams);

			// 스플래시 데미지를 적용합니다.
			UBoxComponent* TileBoxComponent = Tile->GetBoxComponent();
			if (ensureAlways(TileBoxComponent))
			{
				ApplySmashSplashDamage(TileCenter, TileBoxComponent->GetScaledBoxExtent().X * 2);
			}

			// 타일 트리거 정보를 가지고 타일 트리거 GA를 활성화합니다.
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(SourceCharacter, SMTags::Event::Tile::TileTrigger, Payload);
		}
	}
}

void USMGameplayAbility_Smash::ApplySmashSplashDamage(const FVector& TileLocation, float TileHorizonSize)
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

	// 트리거된 타일 크기 영역내 적을 검사합니다.
	TArray<FOverlapResult> OverlapResults;
	const FVector Start = TileLocation;
	const float HorizenSize = (TileHorizonSize * (MaxTriggerCount - 1)) + TileHorizonSize / 2;
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

			// 팀이 없는 경우 대미지를 가하지 않습니다.
			const ESMTeam TargetTeam = TeamComponent->GetTeam();
			if (TargetTeam == ESMTeam::None)
			{
				continue;
			}

			// 같은 팀의 경우도 대미지를 가하지 않습니다.
			if (SourceTeam == TargetTeam)
			{
				continue;
			}

			// 적에게 대미지를 GE로 적용합니다.
			FGameplayEffectSpecHandle GESpecHandle = MakeOutgoingGameplayEffectSpec(DamageGE);
			FGameplayEffectSpec* GESpec = GESpecHandle.Data.Get();
			if (ensureAlways(GESpec))
			{
				GESpec->SetSetByCallerMagnitude(SMTags::Data::Damage, SmashDamage);
			}

			SourceASC->BP_ApplyGameplayEffectSpecToTarget(GESpecHandle, TargetASC);

			// GC로 피격 이펙트를 재생합니다.
			FGameplayCueParameters GCParams;
			GCParams.Location = TargetCharacter->GetActorLocation();
			GCParams.SourceObject = SplashDamageFX[SourceTeam];

			TargetASC->ExecuteGameplayCue(SMTags::GameplayCue::PlayNiagara, GCParams);
		}
	}

	DrawDebugBox(GetWorld(), Start, CollisionHalfExtend, FColor::Orange, false, 2.0f);
}
