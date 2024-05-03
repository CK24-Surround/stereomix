// Copyright Surround, Inc. All Rights Reserved.


#include "SMGameplayAbility_Catch.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "Characters/SMPlayerCharacter.h"
#include "Components/SMTeamComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Utilities/SMCollision.h"
#include "Utilities/SMLog.h"
#include "AbilitySystem/SMTags.h"
#include "Engine/OverlapResult.h"

USMGameplayAbility_Catch::USMGameplayAbility_Catch()
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;

	ActivationOwnedTags = FGameplayTagContainer(SMTags::Ability::Activation::Catch);

	FGameplayTagContainer BlockedTags;
	BlockedTags.AddTag(SMTags::Character::State::Catch);
	BlockedTags.AddTag(SMTags::Character::State::Caught);
	BlockedTags.AddTag(SMTags::Character::State::Smashed);
	BlockedTags.AddTag(SMTags::Character::State::Smashing);
	BlockedTags.AddTag(SMTags::Character::State::Stun);
	BlockedTags.AddTag(SMTags::Character::State::Immune);
	ActivationBlockedTags = BlockedTags;
}

void USMGameplayAbility_Catch::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ASMPlayerCharacter* SourceCharacter = GetSMPlayerCharacterFromActorInfo();
	if (!ensure(SourceCharacter))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	USMAbilitySystemComponent* SourceASC = GetSMAbilitySystemComponentFromActorInfo();
	if (!ensure(SourceASC))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UAbilityTask_PlayMontageAndWait* PlayMontageAndWaitTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("PlayMontageAndWait"), CatchMontage);
	if (!ensure(PlayMontageAndWaitTask))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// OnComplete는 EndAbility를 호출하기에 적합하지 않아 제외했습니다. BlendOut에 들어간 시점에 몽타주가 캔슬되면 OnComplete가 호출되지 않고, OnCancelled, OnInterrupted도 호출되지 않아 버그가 생길 수 있습니다. 
	PlayMontageAndWaitTask->OnCancelled.AddDynamic(this, &USMGameplayAbility_Catch::OnInterrupted);
	PlayMontageAndWaitTask->OnInterrupted.AddDynamic(this, &USMGameplayAbility_Catch::OnInterrupted);
	PlayMontageAndWaitTask->OnBlendOut.AddDynamic(this, &USMGameplayAbility_Catch::OnComplete);
	PlayMontageAndWaitTask->ReadyForActivation();

	// 마우스 커서 정보는 클라이언트에만 존재합니다.
	// 따라서 클라이언트의 커서정보를 기반으로 서버에 잡기 요청을 하기 위해 해당 로직은 클라이언트에서만 실행되어야합니다.
	if (!ActorInfo->IsNetAuthority())
	{
		StartLocation = SourceCharacter->GetActorLocation();
		TargetLocation = SourceCharacter->GetCursorTargetingPoint();

		// 애님 노티파이를 기다리고 노티파이가 호출되면 잡기를 요청합니다.
		UAbilityTask_WaitGameplayEvent* WaitGameplayEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, SMTags::Event::AnimNotify::Catch);
		if (!ensure(WaitGameplayEventTask))
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
			return;
		}

		WaitGameplayEventTask->EventReceived.AddDynamic(this, &USMGameplayAbility_Catch::OnCatchAnimNotify);
		WaitGameplayEventTask->ReadyForActivation();
	}

	CommitAbility(Handle, ActorInfo, ActivationInfo);

	// 잡기 시전 시 이펙트 입니다.
	FGameplayCueParameters GCParams;
	GCParams.Location = SourceCharacter->GetActorLocation();
	GCParams.Normal = SourceCharacter->GetActorRotation().Vector();
	SourceASC->ExecuteGameplayCue(SMTags::GameplayCue::Catch, GCParams);
}

void USMGameplayAbility_Catch::OnInterrupted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void USMGameplayAbility_Catch::OnComplete()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void USMGameplayAbility_Catch::OnCatchAnimNotify(FGameplayEventData Payload)
{
	ServerRPCRequestCatchProcess(StartLocation, TargetLocation);
}

void USMGameplayAbility_Catch::ServerRPCRequestCatchProcess_Implementation(const FVector_NetQuantize10& InStartLocation, const FVector_NetQuantize10& InCursorLocation)
{
	// 클라이언트와 동일한 값을 사용할 수 있도록 저장합니다.
	StartLocation = InStartLocation;
	TargetLocation = InCursorLocation;

	ASMPlayerCharacter* SourceCharacter = GetSMPlayerCharacterFromActorInfo();
	if (!ensure(SourceCharacter))
	{
		return;
	}

	// 일정 범위내에 있는 캐릭터를 대상으로 충돌 검사를 수행합니다.
	TArray<FOverlapResult> OverlapResults;
	const FCollisionShape CollisionShape = FCollisionShape::MakeSphere(MaxDistance);
	const FCollisionQueryParams Params(SCENE_QUERY_STAT(Hold), false, SourceCharacter);
	bool bSuccess = GetWorld()->OverlapMultiByChannel(OverlapResults, InStartLocation, FQuat::Identity, SMCollisionTraceChannel::Action, CollisionShape, Params);

	if (bSuccess)
	{
		bSuccess = false;

		// OverlapResults에서 잡을 수 있는 캐릭터만 추려냅니다. 그리고 잡을 수 있는 대상이 있다면 아래 로직을 진행합니다.
		TArray<ASMPlayerCharacter*> CatchableCharacters;
		if (GetCatchableCharacters(OverlapResults, CatchableCharacters))
		{
			// 커서 위치에 가장 가까운 캐릭터를 TargetCharacter에 담습니다.
			ASMPlayerCharacter* TargetCharacter = GetClosestCharacterFromLocation(CatchableCharacters, InCursorLocation);
			if (ensure(TargetCharacter))
			{
				USMAbilitySystemComponent* SourceASC = GetSMAbilitySystemComponentFromActorInfo();
				USMAbilitySystemComponent* TargetASC = Cast<USMAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetCharacter));
				if (ensure(SourceASC && TargetASC))
				{
					// 이펙트 재생을 위한 어태치 전에 타겟의 위치를 저장해둡니다.
					const FVector BeforeAttachTargetLocation = TargetCharacter->GetActorLocation();

					// 소스는 잡기, 타겟은 잡힌 상태 태그를 추가해줍니다.
					SourceCharacter->SetCatchCharacter(TargetCharacter);
					SourceASC->AddTag(SMTags::Character::State::Catch);

					TargetCharacter->SetCaughtCharacter(SourceCharacter);
					TargetASC->AddTag(SMTags::Character::State::Caught);

					// 어태치하고 잡힌 대상은 잡히기 GA를 활성화합니다. 만약에 실패한다면 적용사항들을 롤백합니다. 일반적으로 성공해야만 합니다.
					const bool bSuccessAttach = AttachTargetCharacter(TargetCharacter);
					if (ensure(bSuccessAttach))
					{
						TargetASC->TryActivateAbilitiesByTag(FGameplayTagContainer(SMTags::Ability::Caught));
						bSuccess = true;
					}
					else
					{
						SourceCharacter->SetCatchCharacter(nullptr);
						SourceASC->RemoveTag(SMTags::Character::State::Catch);

						TargetCharacter->SetCaughtCharacter(nullptr);
						TargetASC->RemoveTag(SMTags::Character::State::Caught);
					}

					// 잡기 적중 성공 시 이펙트입니다.
					FGameplayCueParameters GCParams;
					const FVector SourceLocation = SourceCharacter->GetActorLocation();
					GCParams.Location = SourceCharacter->GetActorLocation();
					GCParams.Normal = (BeforeAttachTargetLocation - SourceLocation).GetSafeNormal();
					SourceASC->ExecuteGameplayCue(SMTags::GameplayCue::CatchHit, GCParams);
				}
			}
		}
	}

#if UE_ENABLE_DEBUG_DRAWING
	if (bShowDebug)
	{
		const FColor Color = bSuccess ? FColor::Green : FColor::Red;
		DrawDebugSphere(GetWorld(), InStartLocation, MaxDistance, 16, Color, false, 2.0f);
	}
#endif
}

bool USMGameplayAbility_Catch::GetCatchableCharacters(const TArray<FOverlapResult>& InOverlapResults, TArray<ASMPlayerCharacter*>& OutCatchableCharacters)
{
	ASMPlayerCharacter* SourceCharacter = GetSMPlayerCharacterFromActorInfo();
	if (!ensure(SourceCharacter))
	{
		return false;
	}

	USMTeamComponent* SourceTeamComponent = SourceCharacter->GetTeamComponent();
	if (!ensure(SourceTeamComponent))
	{
		return false;
	}

	ESMTeam SourceCharacterTeam = SourceTeamComponent->GetTeam();

	// 만약 잡기 시전하는 캐릭터가 무소속이면 아무도 잡지 않습니다.
	if (SourceCharacterTeam == ESMTeam::None)
	{
		return false;
	}

	OutCatchableCharacters.Empty();

	bool bIsCanCatchableCharacter = false;
	for (const auto& OverlapResult : InOverlapResults)
	{
		// 플레이어 캐릭터만 추려냅니다.
		ASMPlayerCharacter* TargetCharacter = Cast<ASMPlayerCharacter>(OverlapResult.GetActor());
		if (TargetCharacter)
		{
			// 무조건 잡을 수 있는 거리 밖에 있다면 각도로 잡을 수 있는지 없는지 추려냅니다.
			const float DistanceSqaured = FVector::DistSquared(StartLocation, TargetCharacter->GetActorLocation());
			if (DistanceSqaured > FMath::Square(UnconditionallyCatchDistance))
			{
				const FVector SourceDirection = (TargetLocation - StartLocation).GetSafeNormal();
				const FVector TargetDirection = (TargetCharacter->GetActorLocation() - StartLocation).GetSafeNormal();
				const float Radian = FMath::Acos(SourceDirection.Dot(TargetDirection));
				const float HalfLimitRadian = FMath::DegreesToRadians(LimitDegree / 2.0f);
				if (Radian > HalfLimitRadian)
				{
					continue;
				}
			}

			// 태그를 기반으로 추려냅니다. 일반적으로 TargetCharacter는 AStereoMixPlayerCharacter이기 때문에 TargetASC가 유효해야합니다.
			const UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetCharacter);
			if (ensure(TargetASC))
			{
				USMTeamComponent* TargetTeamComponent = TargetCharacter->GetTeamComponent();
				if (!ensure(TargetTeamComponent))
				{
					continue;
				}

				const ESMTeam TargetTeam = TargetTeamComponent->GetTeam();

				// 대상이 무소속이라면 잡기를 무시합니다. 사실상 무소속은 대미지를 받지 않기 때문에 기절할 일은 없긴하지만 만약을 위한 예외처리입니다.
				if (TargetTeam == ESMTeam::None)
				{
					continue;
				}

				// 팀이 같다면 잡기를 무시합니다.
				if (SourceCharacterTeam == TargetTeam)
				{
					continue;
				}

				// 만약 1번이라도 잡았던 대상이었다면 무시합니다.
				bool bIsCaptureCharacter = false;
				for (const auto& CapturedCharcter : TargetCharacter->CapturedCharcters)
				{
					if (!CapturedCharcter.Get())
					{
						continue;
					}

					if (CapturedCharcter == SourceCharacter)
					{
						bIsCaptureCharacter = true;
						continue;
					}
				}

				if (bIsCaptureCharacter)
				{
					continue;
				}

				if (TargetASC->HasMatchingGameplayTag(SMTags::Character::State::Uncatchable))
				{
					continue;
				}
				else if (!TargetASC->HasAnyMatchingGameplayTags(CatchableTags))
				{
					continue;
				}

				OutCatchableCharacters.Add(TargetCharacter);
				bIsCanCatchableCharacter = true;
			}
		}
	}

	return bIsCanCatchableCharacter;
}

ASMPlayerCharacter* USMGameplayAbility_Catch::GetClosestCharacterFromLocation(const TArray<ASMPlayerCharacter*>& InCharacters, const FVector& InLocation)
{
	ASMPlayerCharacter* TargetCharacter = nullptr;
	float ClosestDistanceSquaredToCursor = MAX_FLT;
	for (const auto& Character : InCharacters)
	{
		const float DistanceSquared = FVector::DistSquared(Character->GetActorLocation(), InLocation);
		if (DistanceSquared < ClosestDistanceSquaredToCursor)
		{
			ClosestDistanceSquaredToCursor = DistanceSquared;
			TargetCharacter = Character;
		}
	}

	return TargetCharacter;
}

bool USMGameplayAbility_Catch::AttachTargetCharacter(ASMPlayerCharacter* InTargetCharacter)
{
	bool bSuccess = true;
	ASMPlayerCharacter* SourceCharacter = GetSMPlayerCharacterFromActorInfo();
	if (ensure(SourceCharacter && InTargetCharacter))
	{
		// 어태치합니다. 디버깅을 위해 단언을 수행합니다. 어태치 후 상대 회전을 0으로 정렬해줍니다.
		if (InTargetCharacter->AttachToComponent(SourceCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("CatchSlotSocket")))
		{
			// 타겟의 잡힌 목록에 넣습니다.
			InTargetCharacter->CapturedCharcters.AddUnique(SourceCharacter);

			// 캐릭터 상태 위젯을 숨깁니다.
			InTargetCharacter->SetCharacterStateVisibility(false);

			// 충돌판정, 움직임을 잠급니다.
			InTargetCharacter->SetEnableCollision(false);
			InTargetCharacter->SetEnableMovement(false);

			// 타겟의 위치 보정 무시를 활성화합니다.
			UCharacterMovementComponent* TargetMovement = InTargetCharacter->GetCharacterMovement();
			if (ensure(TargetMovement))
			{
				TargetMovement->bIgnoreClientMovementErrorChecksAndCorrection = true;
			}

			// 루트와 메시 사이에 생기는 오프셋 차이를 수정합니다.
			InTargetCharacter->MulticastRPCApplyAttachOffset();

			// 타겟 플레이어의 카메라 뷰를 소스 캐릭터의 카메라 뷰로 전환합니다.
			APlayerController* TargetPlayerController = Cast<APlayerController>(InTargetCharacter->GetController());
			if (ensure(TargetPlayerController))
			{
				TargetPlayerController->SetViewTargetWithBlend(SourceCharacter, 1.0f, VTBlend_Cubic);
			}
		}
		else
		{
			bSuccess = false;
		}
	}

	return bSuccess;
}
