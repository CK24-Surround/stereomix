// Copyright Surround, Inc. All Rights Reserved.


#include "SMGA_Hold.h"

#include "Engine/OverlapResult.h"
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

	ActivationOwnedTags = FGameplayTagContainer(SMTags::Ability::Activation::Hold);

	FGameplayTagContainer BlockedTags;
	BlockedTags.AddTag(SMTags::Character::State::Hold);
	BlockedTags.AddTag(SMTags::Character::State::Holded);
	BlockedTags.AddTag(SMTags::Character::State::NoiseBreaked);
	BlockedTags.AddTag(SMTags::Character::State::NoiseBreaking);
	BlockedTags.AddTag(SMTags::Character::State::Neutralize);
	BlockedTags.AddTag(SMTags::Character::State::Immune);
	BlockedTags.AddTag(SMTags::Character::State::Charge);
	BlockedTags.AddTag(SMTags::Character::State::ImpactArrow);
	ActivationBlockedTags = BlockedTags;
}

void USMGA_Hold::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ASMPlayerCharacterBase* SourceCharacter = GetAvatarActor<ASMPlayerCharacterBase>();
	if (!ensureAlways(SourceCharacter))
	{
		EndAbilityByCancel();
		return;
	}

	USMAbilitySystemComponent* SourceASC = GetASC<USMAbilitySystemComponent>();
	if (!ensureAlways(SourceASC))
	{
		EndAbilityByCancel();
		return;
	}

	const USMPlayerCharacterDataAsset* SourceDataAsset = GetDataAsset();
	if (!ensureAlways(SourceDataAsset))
	{
		return;
	}

	ESMTeam SourceTeam = SourceCharacter->GetTeam();
	CachedCatchMontage = SourceDataAsset->HoldMontage[SourceTeam];

	// 언제든 다른 상태로 인해 끊길 수 있는 애니메이션이기에 bAllowInterruptAfterBlendOut을 활성화 해 언제 끊기던 OnInterrupted가 브로드 캐스트 되도록합니다.
	UAbilityTask_PlayMontageAndWait* PlayMontageAndWaitTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("PlayMontageAndWait"), CachedCatchMontage);
	if (!ensureAlways(PlayMontageAndWaitTask))
	{
		EndAbilityByCancel();
		return;
	}
	// 클라이언트와 서버 각각 애니메이션이 종료되면 스스로 종료하도록 합니다.
	PlayMontageAndWaitTask->OnCancelled.AddDynamic(this, &ThisClass::K2_EndAbilityLocally);
	PlayMontageAndWaitTask->OnInterrupted.AddDynamic(this, &ThisClass::K2_EndAbilityLocally);
	PlayMontageAndWaitTask->OnBlendOut.AddDynamic(this, &ThisClass::K2_EndAbilityLocally);
	PlayMontageAndWaitTask->OnCompleted.AddDynamic(this, &ThisClass::K2_EndAbilityLocally);
	PlayMontageAndWaitTask->ReadyForActivation();

	CommitAbility(Handle, ActorInfo, ActivationInfo);

	// 잡기 시전 시 이펙트 입니다.
	FGameplayCueParameters GCParams;
	GCParams.Location = SourceCharacter->GetActorLocation();
	GCParams.Normal = SourceCharacter->GetActorRotation().Vector();
	SourceASC->ExecuteGameplayCue(SMTags::GameplayCue::Hold, GCParams);

	// 만약 무소속이라면 뒤의 로직을 실행하지 않습니다. 즉 애니메이션과 이펙트만 실행됩니다.
	if (SourceTeam == ESMTeam::None)
	{
		EndAbilityByCancel();
		return;
	}

	// 마우스 커서 정보는 클라이언트에만 존재합니다.
	// 따라서 클라이언트의 커서정보를 기반으로 서버에 잡기 요청을 합니다. 해당 로직은 클라이언트에서만 실행되어야합니다.
	if (!K2_HasAuthority())
	{
		// 커서위치는 즉시 저장합니다. 시작 위치는 ServerRPC를 호출할때 저장합니다.
		TargetLocation = SourceCharacter->GetCursorTargetingPoint();

		// 애님 노티파이를 기다리고 노티파이가 호출되면 잡기를 요청합니다.
		UAbilityTask_WaitGameplayEvent* WaitGameplayEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, SMTags::Event::AnimNotify::Hold);
		if (!ensureAlways(WaitGameplayEventTask))
		{
			EndAbilityByCancel();
			return;
		}
		WaitGameplayEventTask->EventReceived.AddDynamic(this, &ThisClass::OnHoldAnimNotifyTrigger);
		WaitGameplayEventTask->ReadyForActivation();
	}
}

void USMGA_Hold::OnHoldAnimNotifyTrigger(FGameplayEventData Payload)
{
	ASMPlayerCharacterBase* SourceCharacter = GetAvatarActor<ASMPlayerCharacterBase>();
	if (!ensureAlways(SourceCharacter))
	{
		EndAbilityByCancel();
		return;
	}

	// 시전 위치를 저장하고 서버에 잡기 요청을 보냅니다.
	StartLocation = SourceCharacter->GetActorLocation();
	ServerRPCRequestCatch(StartLocation, TargetLocation);
}

void USMGA_Hold::ServerRPCRequestCatch_Implementation(const FVector_NetQuantize10& InStartLocation, const FVector_NetQuantize10& InCursorLocation)
{
	ASMPlayerCharacterBase* SourceCharacter = GetAvatarActor<ASMPlayerCharacterBase>();
	if (!ensureAlways(SourceCharacter))
	{
		EndAbilityByCancel();
		return;
	}

	USMAbilitySystemComponent* SourceASC = GetASC<USMAbilitySystemComponent>();
	if (!ensureAlways(SourceASC))
	{
		EndAbilityByCancel();
		return;
	}

	USMHIC_Character* SourceHIC = GetHIC<USMHIC_Character>();
	if (!ensureAlways(SourceHIC))
	{
		EndAbilityByCancel();
		return;
	}

	// 클라이언트와 동일한 값을 사용할 수 있도록 저장합니다.
	StartLocation = InStartLocation;
	TargetLocation = InCursorLocation;

	// 일정 범위내에 있는 캐릭터를 대상으로 충돌 검사를 수행합니다.
	TArray<FOverlapResult> OverlapResults;
	const FCollisionShape CollisionShape = FCollisionShape::MakeSphere(MaxDistance);
	const FCollisionQueryParams Params(SCENE_QUERY_STAT(Hold), false, SourceCharacter);
	bool bSuccess = GetWorld()->OverlapMultiByChannel(OverlapResults, StartLocation, FQuat::Identity, SMCollisionTraceChannel::Action, CollisionShape, Params);
	if (bSuccess)
	{
		bSuccess = false;

		AActor* TargetActor = GetMostSuitableCatchableActor(OverlapResults);
		if (TargetActor)
		{
			// 이미 잡기 가능한 대상이므로 null체크를 다시 수행할 필요는 없습니다.
			USMHoldInteractionComponent* TargetHIC = USMHoldInteractionBlueprintLibrary::GetHoldInteractionComponent(TargetActor);

			// 이펙트 재생을 위해 잡기 전 타겟의 위치를 저장해둡니다.
			const FVector TargetLocationBeforeCatch = TargetActor->GetActorLocation();

			// 타겟의 잡히기 로직을 실행합니다.
			TargetHIC->OnHolded(SourceCharacter);

			// 잡은 대상을 저장하고 자신의 상태를 잡기로 변경합니다. 예외처리를 위해 잡은 액터의 파괴 시 이벤트에 바인드합니다.
			SourceHIC->SetActorIAmHolding(TargetActor);
			TargetActor->OnDestroyed.AddDynamic(SourceHIC, &USMHIC_Character::OnDestroyedIAmHoldingActor);

			// 잡기 적중에 성공하여 성공 이펙트를 재생합니다.
			FGameplayCueParameters GCParams;
			const FVector SourceLocation = SourceCharacter->GetActorLocation();
			GCParams.Location = SourceCharacter->GetActorLocation();
			GCParams.Normal = (TargetLocationBeforeCatch - SourceLocation).GetSafeNormal();
			SourceASC->ExecuteGameplayCue(SMTags::GameplayCue::CatchHit, GCParams);

			bSuccess = true;

			// 잡기 버프를 활성화합니다. 이동속도가 증가합니다.
			if (Cast<ASMPlayerCharacterBase>(SourceHIC->GetActorIAmHolding()))
			{
				// SourceASC->TryActivateAbilitiesByTag(FGameplayTagContainer(SMTags::Ability::CatchBuff));
			}

			// 해당 클라이언트의 인디케이터를 제거합니다.
			SourceCharacter->ClientRPCRemoveScreendIndicatorToSelf(TargetActor);
		}
	}

	// 잡기 성공 여부에 따라 애니메이션을 재생합니다.
	if (bSuccess)
	{
		ClientRPCPlayMontage(CachedCatchMontage, 1.0F, TEXT("Success"));
		MontageJumpToSection(TEXT("Success"));
	}
	else
	{
		ClientRPCPlayMontage(CachedCatchMontage, 1.0F, TEXT("Fail"));
		MontageJumpToSection(TEXT("Fail"));
	}

#if UE_ENABLE_DEBUG_DRAWING
	if (bShowDebug)
	{
		const FColor Color = bSuccess ? FColor::Green : FColor::Red;
		DrawDebugSphere(GetWorld(), InStartLocation, MaxDistance, 16, Color, false, 2.0f);
	}
#endif
}

AActor* USMGA_Hold::GetMostSuitableCatchableActor(const TArray<FOverlapResult>& InOverlapResults)
{
	ASMPlayerCharacterBase* SourceCharacter = GetAvatarActor<ASMPlayerCharacterBase>();
	if (!ensureAlways(SourceCharacter))
	{
		return nullptr;
	}

	// OverlapResults에서 잡을 수 있는 캐릭터만 추려냅니다.
	TArray<AActor*> CatchableActors;
	if (!GetCatchableActors(InOverlapResults, CatchableActors))
	{
		return nullptr;
	}

	// 커서 위치에 가장 가까운 액터를 반환합니다.
	AActor* TargetActor = GetClosestActorFromLocation(CatchableActors, TargetLocation);
	return TargetActor;
}

bool USMGA_Hold::GetCatchableActors(const TArray<FOverlapResult>& InOverlapResults, TArray<AActor*>& OutCatchableActors)
{
	ASMPlayerCharacterBase* SourceCharacter = GetAvatarActor<ASMPlayerCharacterBase>();
	if (!ensureAlways(SourceCharacter))
	{
		return false;
	}

	// 출력 배열을 초기화합니다.
	OutCatchableActors.Empty();

	bool bIsCanCatchableCharacter = false;
	for (const auto& OverlapResult : InOverlapResults)
	{
		// 잡을 수 있는 액터만 추려냅니다.
		AActor* TargetActor = OverlapResult.GetActor();
		USMHoldInteractionComponent* TargetCIC = USMHoldInteractionBlueprintLibrary::GetHoldInteractionComponent(TargetActor);
		if (TargetCIC)
		{
			// 유효한 각도내에 있는지 체크합니다.
			if (!IsValidateAngle(TargetActor))
			{
				continue;
			}

			// 잡을 수 있는 상태인지 체크합니다.
			if (!TargetCIC->CanHolded(SourceCharacter))
			{
				continue;
			}

			OutCatchableActors.Add(TargetActor);
			bIsCanCatchableCharacter = true;
		}
	}

	return bIsCanCatchableCharacter;
}

bool USMGA_Hold::IsValidateAngle(const AActor* TargetActor)
{
	// 무조건 잡기 거리 이내에 타겟이 존재하면 각도 조건을 무시하고 유효한 각도로 취급합니다.
	const float DistanceSqaured = FVector::DistSquared(StartLocation, TargetActor->GetActorLocation());
	if (DistanceSqaured <= FMath::Square(UnconditionallyCatchDistance))
	{
		return true;
	}

	// 각도 조건을 통해 잡을 수 있는지 없는지 추려냅니다.
	const FVector SourceDirection = (TargetLocation - StartLocation).GetSafeNormal();
	const FVector TargetDirection = (TargetActor->GetActorLocation() - StartLocation).GetSafeNormal();
	const float TargetRadian = FMath::Acos(SourceDirection.Dot(TargetDirection));
	const float HalfLimitRadian = FMath::DegreesToRadians(LimitDegree / 2.0f);

	// LimitDegree각도 내부라면 유효한 각도입니다.
	if (TargetRadian <= HalfLimitRadian)
	{
		return true;
	}

	return false;
}

AActor* USMGA_Hold::GetClosestActorFromLocation(const TArray<AActor*>& InActors, const FVector& InLocation)
{
	AActor* TargetActor = nullptr;
	float ClosestDistanceSquaredToCursor = MAX_FLT;
	for (const auto& Actor : InActors)
	{
		const float DistanceSquared = FVector::DistSquared(Actor->GetActorLocation(), InLocation);
		if (DistanceSquared < ClosestDistanceSquaredToCursor)
		{
			ClosestDistanceSquaredToCursor = DistanceSquared;
			TargetActor = Actor;
		}
	}

	return TargetActor;
}
