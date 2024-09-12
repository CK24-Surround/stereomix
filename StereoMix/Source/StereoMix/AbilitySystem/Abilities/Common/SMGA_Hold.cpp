// Copyright Surround, Inc. All Rights Reserved.


#include "SMGA_Hold.h"

#include "Engine/OverlapResult.h"
#include "Abilities/Tasks/AbilityTask_NetworkSyncPoint.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "Characters/Player/SMPlayerCharacterBase.h"
#include "Data/Character/SMPlayerCharacterDataAsset.h"
#include "HoldInteraction/SMHIC_Character.h"
#include "Utilities/SMCollision.h"

USMGA_Hold::USMGA_Hold()
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;

	ActivationOwnedTags.AddTag(SMTags::Ability::Activation::Hold);

	ActivationBlockedTags.AddTag(SMTags::Character::State::Hold);
	ActivationBlockedTags.AddTag(SMTags::Character::State::Holded);
	ActivationBlockedTags.AddTag(SMTags::Character::State::NoiseBreaked);
	ActivationBlockedTags.AddTag(SMTags::Character::State::NoiseBreaking);
	ActivationBlockedTags.AddTag(SMTags::Character::State::Neutralize);
	ActivationBlockedTags.AddTag(SMTags::Character::State::Immune);
	ActivationBlockedTags.AddTag(SMTags::Character::State::Charge);
	ActivationBlockedTags.AddTag(SMTags::Character::State::ImpactArrow);
}

void USMGA_Hold::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ASMPlayerCharacterBase* SourceCharacter = GetAvatarActor<ASMPlayerCharacterBase>();
	USMAbilitySystemComponent* SourceASC = GetASC<USMAbilitySystemComponent>();
	const USMPlayerCharacterDataAsset* SourceDataAsset = GetDataAsset();
	if (!SourceCharacter || !ensureAlways(SourceASC) || !ensureAlways(SourceDataAsset))
	{
		EndAbilityByCancel();
		return;
	}

	K2_CommitAbility();

	CachedHoldMontage = SourceDataAsset->HoldMontage[SourceCharacter->GetTeam()];

	// 언제든 다른 상태로 인해 끊길 수 있는 애니메이션이기에 bAllowInterruptAfterBlendOut을 활성화 해 언제 끊기던 OnInterrupted가 브로드 캐스트 되도록합니다.
	// 클라이언트와 서버 각각 애니메이션이 종료되면 스스로 종료하도록 합니다.
	NET_LOG(SourceCharacter, Warning, TEXT("잡기 시작"));
	UAbilityTask_PlayMontageAndWait* PlayMontageAndWaitTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("PlayMontageAndWait"), CachedHoldMontage);
	// PlayMontageAndWaitTask->OnCancelled.AddDynamic(this, &ThisClass::K2_EndAbility);
	// PlayMontageAndWaitTask->OnBlendOut.AddDynamic(this, &ThisClass::K2_EndAbility);
	PlayMontageAndWaitTask->OnInterrupted.AddDynamic(this, &ThisClass::K2_EndAbility);
	PlayMontageAndWaitTask->OnCompleted.AddDynamic(this, &ThisClass::K2_EndAbility);
	PlayMontageAndWaitTask->ReadyForActivation();

	// 잡기 시전 시 이펙트 입니다.
	// FGameplayCueParameters GCParams;
	// GCParams.Location = SourceCharacter->GetActorLocation();
	// GCParams.Normal = SourceCharacter->GetActorRotation().Vector();
	// SourceASC->ExecuteGameplayCue(SMTags::GameplayCue::Hold, GCParams);

	if (IsLocallyControlled())
	{
		// 커서위치는 즉시 저장합니다. 시작 위치는 ServerRPC를 호출할때 저장합니다.
		CursorLocation = SourceCharacter->GetLocationFromCursor();

		// 애님 노티파이를 기다리고 노티파이가 호출되면 잡기를 요청합니다.
		UAbilityTask_WaitGameplayEvent* WaitGameplayEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, SMTags::Event::AnimNotify::Hold);
		WaitGameplayEventTask->EventReceived.AddDynamic(this, &ThisClass::OnHoldAnimNotifyTrigger);
		WaitGameplayEventTask->ReadyForActivation();

		// 서버의 잡기 로직이 종료될때까지 대기합니다.
	}
}

void USMGA_Hold::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	NET_LOG(GetAvatarActor(), Warning, TEXT("잡기 종료"));
	bSuccessHold = false;

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USMGA_Hold::OnHoldAnimNotifyTrigger(FGameplayEventData Payload)
{
	ASMPlayerCharacterBase* SourceCharacter = GetAvatarActor<ASMPlayerCharacterBase>();
	if (!SourceCharacter)
	{
		EndAbilityByCancel();
		return;
	}

	// 시전 위치를 저장하고 서버에 잡기 요청을 보냅니다.
	StartLocation = SourceCharacter->GetActorLocation();
	ServerRPCRequestCatch(StartLocation, CursorLocation);
}

void USMGA_Hold::ServerRPCRequestCatch_Implementation(const FVector_NetQuantize10& InStartLocation, const FVector_NetQuantize10& InCursorLocation)
{
	ASMPlayerCharacterBase* SourceCharacter = GetAvatarActor<ASMPlayerCharacterBase>();
	USMAbilitySystemComponent* SourceASC = GetASC<USMAbilitySystemComponent>();
	USMHIC_Character* SourceHIC = GetHIC<USMHIC_Character>();
	if (!SourceCharacter || !ensureAlways(SourceASC) || !ensureAlways(SourceHIC))
	{
		EndAbilityByCancel();
		return;
	}

	// 로컬 컨트롤과 동일한 값을 사용할 수 있도록 서버에 저장합니다.
	StartLocation = InStartLocation;
	CursorLocation = InCursorLocation;

	// 일정 범위내에 있는 캐릭터를 대상으로 충돌 검사를 수행합니다.
	TArray<FOverlapResult> OverlapResults;
	const FCollisionShape CollisionShape = FCollisionShape::MakeSphere(MaxDistance);
	const FCollisionQueryParams Params(SCENE_QUERY_STAT(Hold), false, SourceCharacter);
	if (GetWorld()->OverlapMultiByChannel(OverlapResults, StartLocation, FQuat::Identity, SMCollisionTraceChannel::Action, CollisionShape, Params))
	{
		AActor* TargetActor = GetOptimalHoldableActor(OverlapResults);
		USMHoldInteractionComponent* TargetHIC = USMHoldInteractionBlueprintLibrary::GetHoldInteractionComponent(TargetActor);
		if (TargetHIC)
		{
			bSuccessHold = true;

			// 이펙트 재생을 위해 잡기 전 타겟의 위치를 저장해둡니다.
			// const FVector TargetLocationBeforeHold = TargetActor->GetActorLocation();

			// 타겟의 잡히기 로직을 실행합니다.
			TargetHIC->OnHolded(SourceCharacter);

			// 잡은 대상을 저장하고 자신의 상태를 잡기로 변경합니다. 예외처리를 위해 잡은 액터의 파괴 시 수행해야할 이벤트를 바인드합니다.
			SourceHIC->SetActorIAmHolding(TargetActor);
			TargetActor->OnDestroyed.AddDynamic(SourceHIC, &USMHIC_Character::OnDestroyedIAmHoldingActor);

			// 잡기 적중에 성공하여 성공 이펙트를 재생합니다.
			// FGameplayCueParameters GCParams;
			// const FVector SourceLocation = SourceCharacter->GetActorLocation();
			// GCParams.Location = SourceCharacter->GetActorLocation();
			// GCParams.Normal = (TargetLocationBeforeHold - SourceLocation).GetSafeNormal();
			// SourceASC->ExecuteGameplayCue(SMTags::GameplayCue::CatchHit, GCParams);

			// 잡기 버프를 활성화합니다. 캐릭터를 잡은 경우 이동속도가 증가합니다.
			if (Cast<ASMPlayerCharacterBase>(SourceHIC->GetActorIAmHolding()))
			{
				// SourceASC->TryActivateAbilitiesByTag(FGameplayTagContainer(SMTags::Ability::CatchBuff));
			}

			// 로컬에서 타겟 인디케이터를 제거합니다.
			SourceCharacter->ClientRPCRemoveScreendIndicatorToSelf(TargetActor);
		}
	}

	NET_LOG(SourceCharacter, Warning, TEXT("잡기 완료"));
	ClientRPCSendHoldResult(bSuccessHold);
	WaitHoldResult();

#if UE_ENABLE_DEBUG_DRAWING
	if (bShowDebug)
	{
		const FColor Color = bSuccessHold ? FColor::Green : FColor::Red;
		DrawDebugSphere(GetWorld(), InStartLocation, MaxDistance, 16, Color, false, 2.0f);
	}
#endif
}

AActor* USMGA_Hold::GetOptimalHoldableActor(const TArray<FOverlapResult>& InOverlapResults)
{
	ASMPlayerCharacterBase* SourceCharacter = GetAvatarActor<ASMPlayerCharacterBase>();
	if (!SourceCharacter)
	{
		return nullptr;
	}

	// OverlapResults에서 잡을 수 있는 캐릭터만 추려냅니다.
	TArray<AActor*> CatchableActors;
	if (!GetHoldableActors(InOverlapResults, CatchableActors))
	{
		return nullptr;
	}

	// 커서 위치에 가장 가까운 액터를 반환합니다.
	AActor* TargetActor = GetClosestActorFromLocation(CatchableActors, CursorLocation);
	return TargetActor;
}

bool USMGA_Hold::GetHoldableActors(const TArray<FOverlapResult>& InOverlapResults, TArray<AActor*>& OutCatchableActors)
{
	OutCatchableActors.Empty();
	bool bCanHoldableActor = false;

	ASMPlayerCharacterBase* SourceCharacter = GetAvatarActor<ASMPlayerCharacterBase>();
	if (!ensureAlways(SourceCharacter))
	{
		return false;
	}

	for (const auto& OverlapResult : InOverlapResults)
	{
		// 잡을 수 있는 액터만 추려냅니다.
		AActor* TargetActor = OverlapResult.GetActor();
		USMHoldInteractionComponent* TargetHIC = USMHoldInteractionBlueprintLibrary::GetHoldInteractionComponent(TargetActor);
		if (TargetHIC)
		{
			// 유효한 각도내에 있는지 체크합니다.
			if (!IsValidateAngle(TargetActor))
			{
				continue;
			}

			// 잡을 수 있는 상태인지 체크합니다.
			if (!TargetHIC->CanHolded(SourceCharacter))
			{
				continue;
			}

			OutCatchableActors.Add(TargetActor);
			bCanHoldableActor = true;
		}
	}

	return bCanHoldableActor;
}

bool USMGA_Hold::IsValidateAngle(const AActor* TargetActor)
{
	// 무조건 잡기 거리 이내에 타겟이 존재하면 각도 조건을 무시하고 유효한 각도로 취급합니다.
	const float DistanceSqaured = FVector::DistSquared(StartLocation, TargetActor->GetActorLocation());
	if (DistanceSqaured <= FMath::Square(UnconditionalHoldDistance))
	{
		return true;
	}

	// 각도 조건을 통해 잡을 수 있는지 없는지 추려냅니다.
	const FVector SourceToCursorDirection = (CursorLocation - StartLocation).GetSafeNormal();
	const FVector SourceToTargetDirection = (TargetActor->GetActorLocation() - StartLocation).GetSafeNormal();

	const float DotProduct = SourceToCursorDirection.Dot(SourceToTargetDirection);
	const float RadianDifference = FMath::Acos(DotProduct);

	// LimitDegree각도 내부라면 유효한 각도입니다.
	const float HalfLimitRadian = FMath::DegreesToRadians(LimitDegree / 2.0f);
	if (RadianDifference <= HalfLimitRadian)
	{
		return true;
	}

	return false;
}

AActor* USMGA_Hold::GetClosestActorFromLocation(const TArray<AActor*>& InActors, const FVector& InLocation)
{
	AActor* ClosestActor = nullptr;
	float ClosestDistanceSquaredToCursor = MAX_FLT;
	for (const auto& Actor : InActors)
	{
		if (!Actor)
		{
			continue;
		}

		const FVector ActorLocation = Actor->GetActorLocation();
		const float DistanceSquared = FVector::DistSquared(ActorLocation, InLocation);
		if (DistanceSquared < ClosestDistanceSquaredToCursor)
		{
			ClosestActor = Actor;
			ClosestDistanceSquaredToCursor = DistanceSquared;
		}
	}

	return ClosestActor;
}

void USMGA_Hold::ClientRPCSendHoldResult_Implementation(bool bNewSuccessHold)
{
	bSuccessHold = bNewSuccessHold;
	WaitHoldResult();
}

void USMGA_Hold::WaitHoldResult()
{
	NET_LOG(GetAvatarActor(), Warning, TEXT("잡기 결과 대기"));
	UAbilityTask_NetworkSyncPoint* SyncPoint = UAbilityTask_NetworkSyncPoint::WaitNetSync(this, EAbilityTaskNetSyncType::BothWait);
	SyncPoint->OnSync.AddDynamic(this, &ThisClass::PlayResultMontage);
	SyncPoint->ReadyForActivation();
}

void USMGA_Hold::PlayResultMontage()
{
	NET_LOG(GetAvatarActor(), Warning, TEXT("잡기 결과: %d"), bSuccessHold);
	if (bSuccessHold)
	{
		const FName SectionName = TEXT("Success");
		MontageJumpToSection(SectionName);
	}
	else
	{
		const FName SectionName = TEXT("Fail");
		MontageJumpToSection(SectionName);
	}
}
