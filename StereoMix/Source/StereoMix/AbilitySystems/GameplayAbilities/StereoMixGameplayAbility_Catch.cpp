// Fill out your copyright notice in the Description page of Project Settings.


#include "StereoMixGameplayAbility_Catch.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Characters/StereoMixPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Utilities/StereoMixCollision.h"
#include "Utilities/StereoMixeLog.h"
#include "Utilities/StereoMixTag.h"

UStereoMixGameplayAbility_Catch::UStereoMixGameplayAbility_Catch()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;
}

void UStereoMixGameplayAbility_Catch::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!ensure(ActorInfo))
	{
		return;
	}

	AStereoMixPlayerCharacter* StereoMixCharacter = Cast<AStereoMixPlayerCharacter>(ActorInfo->AvatarActor.Get());
	if (!StereoMixCharacter)
	{
		NET_LOG(ActorInfo->AvatarActor.Get(), Warning, TEXT("호환되지 않은 GA입니다."))
		return;
	}

	UAbilityTask_PlayMontageAndWait* AbilityTaskPlayMontageAndWait = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("PlayMontageAndWait"), CatchMontage);
	if (AbilityTaskPlayMontageAndWait)
	{
		// OnComplete는 EndAbility를 호출하기에 적합하지 않아 제외했습니다. BlendOut에 들어간 시점에 몽타주가 캔슬되면 OnComplete가 호출되지 않고, OnCancelled, OnInterrupted도 호출되지 않아 버그가 생길 수 있습니다. 
		AbilityTaskPlayMontageAndWait->OnCancelled.AddDynamic(this, &UStereoMixGameplayAbility_Catch::OnCancelled);
		AbilityTaskPlayMontageAndWait->OnInterrupted.AddDynamic(this, &UStereoMixGameplayAbility_Catch::OnInterrupted);
		AbilityTaskPlayMontageAndWait->OnBlendOut.AddDynamic(this, &UStereoMixGameplayAbility_Catch::OnBlendOut);
		AbilityTaskPlayMontageAndWait->ReadyForActivation();
	}

	CommitAbility(Handle, ActorInfo, ActivationInfo);

	if (!ActorInfo->IsNetAuthority())
	{
		StartLocation = StereoMixCharacter->GetActorLocation();
		TargetLocation = StereoMixCharacter->GetCursorTargetingPoint();

		UAbilityTask_WaitGameplayEvent* AbilityTaskWaitGameplayEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, StereoMixTag::Event::Character::Catch);
		if (AbilityTaskWaitGameplayEvent)
		{
			AbilityTaskWaitGameplayEvent->EventReceived.AddDynamic(this, &UStereoMixGameplayAbility_Catch::OnEventReceived);
			AbilityTaskWaitGameplayEvent->ReadyForActivation();
		}
	}
}

void UStereoMixGameplayAbility_Catch::OnCancelled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UStereoMixGameplayAbility_Catch::OnInterrupted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UStereoMixGameplayAbility_Catch::OnBlendOut()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UStereoMixGameplayAbility_Catch::OnEventReceived(FGameplayEventData Payload)
{
	if (!CurrentActorInfo)
	{
		return;
	}

	ServerRPCRequestTargetOverlap(StartLocation, TargetLocation);
}

void UStereoMixGameplayAbility_Catch::ServerRPCRequestTargetOverlap_Implementation(const FVector_NetQuantize10& InStartLocation, const FVector_NetQuantize10& InCursorLocation)
{
	if (!ensure(CurrentActorInfo))
	{
		return;
	}

	AStereoMixPlayerCharacter* CasterCharacter = Cast<AStereoMixPlayerCharacter>(CurrentActorInfo->AvatarActor.Get());
	if (!CasterCharacter)
	{
		NET_LOG(nullptr, Error, TEXT("시전자가 유효하지 않습니다."));
		return;
	}

	if (InStartLocation.Z != InCursorLocation.Z)
	{
		NET_LOG(CurrentActorInfo ? CurrentActorInfo->AvatarActor.Get() : nullptr, Warning, TEXT("캐릭터의 위치와 커서의 위치가 평면상에 놓이지 않았습니다. 투사체 시작점: %s 목표 위치: %s"), *StartLocation.ToString(), *InCursorLocation.ToString());
	}

	TArray<FOverlapResult> OverlapResults;
	const float Distance = 300.0f;
	const FCollisionShape Sphere = FCollisionShape::MakeSphere(Distance);
	const FCollisionQueryParams Params(SCENE_QUERY_STAT(Catch), false, CasterCharacter);

	bool bSuccess = GetWorld()->OverlapMultiByChannel(OverlapResults, InStartLocation, FQuat::Identity, StereoMixCollisionTraceChannel::Action, Sphere, Params);

	if (bSuccess)
	{
		bSuccess = false;

		const TArray<AStereoMixPlayerCharacter*> CheckedCharacters = GetCatchableCharacters(OverlapResults);

		AStereoMixPlayerCharacter* TargetCharacter = nullptr;
		float ClosestDistanceSquaredToCursor = MAX_FLT;
		for (const auto& CheckedCharacter : CheckedCharacters)
		{
			const float DistanceSquared = FVector::DistSquared(CheckedCharacter->GetActorLocation(), InCursorLocation);
			if (DistanceSquared < ClosestDistanceSquaredToCursor)
			{
				ClosestDistanceSquaredToCursor = DistanceSquared;
				TargetCharacter = CheckedCharacter;
			}
		}

		if (TargetCharacter)
		{
			bSuccess = true;
			UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
			UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetCharacter);
			if (SourceASC && TargetASC)
			{
				FGameplayEffectContextHandle SourceGEContextHandle = SourceASC->MakeEffectContext();
				if (SourceGEContextHandle.IsValid())
				{
					SourceASC->BP_ApplyGameplayEffectToSelf(AddCatchStateGE, 0.0f, SourceGEContextHandle);
				}
				
				
				FGameplayEffectContextHandle TargetGEContextHandle = TargetASC->MakeEffectContext();
				if (TargetGEContextHandle.IsValid())
				{
					TargetASC->BP_ApplyGameplayEffectToSelf(AddCaughtStateGE, 0.0f, TargetGEContextHandle);
				}

				AttachTargetCharacter(TargetCharacter);
				NET_LOG(CasterCharacter, Log, TEXT("%s가 %s를 잡았습니다."), *CasterCharacter->GetName(), *TargetCharacter->GetName());
			}
			else
			{
				NET_LOG(CasterCharacter, Log, TEXT("잡기 타겟 검출에 성공했으나, Source 혹은 Target의 ASC가 유효하지 않습니다."));
			}
		}
	}

	const FColor Color = bSuccess ? FColor::Green : FColor::Red;
	DrawDebugSphere(GetWorld(), InStartLocation, Distance, 16, Color, false, 2.0f);
}

TArray<AStereoMixPlayerCharacter*> UStereoMixGameplayAbility_Catch::GetCatchableCharacters(const TArray<FOverlapResult>& InOverlapResults)
{
	TArray<AStereoMixPlayerCharacter*> CheckedCharacters;
	for (const auto& OverlapResult : InOverlapResults)
	{
		AStereoMixPlayerCharacter* CatchableCharacter = Cast<AStereoMixPlayerCharacter>(OverlapResult.GetActor());
		if (!CatchableCharacter)
		{
			continue;
		}

		const UAbilitySystemComponent* CatchableASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(CatchableCharacter);
		if (!CatchableASC)
		{
			NET_LOG(nullptr, Warning, TEXT("타겟의 ASC가 유효하지 않습니다."));
			continue;
		}

		// TODO: 여기서 팀이 다르면 걸러줘야합니다.

		// 잡을 수 있는지 확인합니다.
		if (CatchableASC->HasMatchingGameplayTag(StereoMixTag::Character::State::Uncatchable))
		{
			continue;
		}
		else if (!CatchableASC->HasAnyMatchingGameplayTags(CatchableTags))
		{
			continue;
		}

		CheckedCharacters.Add(CatchableCharacter);
	}

	return CheckedCharacters;
}

void UStereoMixGameplayAbility_Catch::AttachTargetCharacter(AStereoMixPlayerCharacter* InTargetCharacter)
{
	const AStereoMixPlayerCharacter* CasterCharacter = Cast<AStereoMixPlayerCharacter>(GetAvatarActorFromActorInfo());
	if (!CasterCharacter)
	{
		return;
	}

	if (!InTargetCharacter)
	{
		return;
	}

	InTargetCharacter->GetCharacterMovement()->bIgnoreClientMovementErrorChecksAndCorrection = true;

	if (InTargetCharacter->AttachToComponent(CasterCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("CatchSocket")))
	{
		NET_LOG(CasterCharacter, Log, TEXT("잡기(어태치) 성공"));
	}

	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InTargetCharacter);
	if (!TargetASC)
	{
		return;
	}

	TargetASC->TryActivateAbilitiesByTag(FGameplayTagContainer(StereoMixTag::Ability::Caught));
}
