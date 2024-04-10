// Fill out your copyright notice in the Description page of Project Settings.


#include "StereoMixGameplayAbility_Catch.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystem/StereoMixAbilitySystemComponent.h"
#include "Characters/StereoMixPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Utilities/StereoMixCollision.h"
#include "Utilities/StereoMixeLog.h"
#include "Utilities/StereoMixTag.h"

UStereoMixGameplayAbility_Catch::UStereoMixGameplayAbility_Catch()
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;
}

void UStereoMixGameplayAbility_Catch::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AStereoMixPlayerCharacter* SourceCharacter = GetStereoMixPlayerCharacterFromActorInfo();
	if (!ensure(SourceCharacter))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 마우스 커서 정보는 클라이언트에만 존재합니다.
	// 따라서 클라이언트의 커서정보를 기반으로 서버에 잡기 요청을 하기 위해 해당 로직은 클라이언트에서만 실행되어야합니다.
	if (!ActorInfo->IsNetAuthority())
	{
		StartLocation = SourceCharacter->GetActorLocation();
		TargetLocation = SourceCharacter->GetCursorTargetingPoint();

		// 애님 노티파이를 기다리고 노티파이가 호출되면 잡기를 요청합니다.
		UAbilityTask_WaitGameplayEvent* AbilityTaskWaitGameplayEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, StereoMixTag::Event::AnimNotify::Catch);
		if (AbilityTaskWaitGameplayEvent)
		{
			AbilityTaskWaitGameplayEvent->EventReceived.AddDynamic(this, &UStereoMixGameplayAbility_Catch::OnHoldAnimNotify);
			AbilityTaskWaitGameplayEvent->ReadyForActivation();
		}
	}

	UAbilityTask_PlayMontageAndWait* AbilityTaskPlayMontageAndWait = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("PlayMontageAndWait"), CatchMontage);
	if (AbilityTaskPlayMontageAndWait)
	{
		CommitAbility(Handle, ActorInfo, ActivationInfo);
		// OnComplete는 EndAbility를 호출하기에 적합하지 않아 제외했습니다. BlendOut에 들어간 시점에 몽타주가 캔슬되면 OnComplete가 호출되지 않고, OnCancelled, OnInterrupted도 호출되지 않아 버그가 생길 수 있습니다. 
		AbilityTaskPlayMontageAndWait->OnCancelled.AddDynamic(this, &UStereoMixGameplayAbility_Catch::OnInterrupted);
		AbilityTaskPlayMontageAndWait->OnInterrupted.AddDynamic(this, &UStereoMixGameplayAbility_Catch::OnInterrupted);
		AbilityTaskPlayMontageAndWait->OnBlendOut.AddDynamic(this, &UStereoMixGameplayAbility_Catch::OnComplete);
		AbilityTaskPlayMontageAndWait->ReadyForActivation();
	}
	else
	{
		// 몽타주 재생에 실패했다면 어빌리티를 즉시 종료합니다.
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
}

void UStereoMixGameplayAbility_Catch::OnInterrupted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UStereoMixGameplayAbility_Catch::OnComplete()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UStereoMixGameplayAbility_Catch::OnHoldAnimNotify(FGameplayEventData Payload)
{
	ServerRPCRequestCatchProcess(StartLocation, TargetLocation);
}

void UStereoMixGameplayAbility_Catch::ServerRPCRequestCatchProcess_Implementation(const FVector_NetQuantize10& InStartLocation, const FVector_NetQuantize10& InCursorLocation)
{
	const AStereoMixPlayerCharacter* SourceCharacter = GetStereoMixPlayerCharacterFromActorInfo();
	if (!ensure(SourceCharacter))
	{
		return;
	}

	// 일정 범위내에 있는 캐릭터를 대상으로 충돌 검사를 수행합니다.
	TArray<FOverlapResult> OverlapResults;
	const float Distance = 300.0f;
	const FCollisionShape CollisionShape = FCollisionShape::MakeSphere(Distance);
	const FCollisionQueryParams Params(SCENE_QUERY_STAT(Hold), false, SourceCharacter);
	bool bSuccess = GetWorld()->OverlapMultiByChannel(OverlapResults, InStartLocation, FQuat::Identity, StereoMixCollisionTraceChannel::Action, CollisionShape, Params);

	if (bSuccess)
	{
		bSuccess = false;

		// OverlapResults에서 잡을 수 있는 캐릭터만 추려냅니다. 그리고 잡을 수 있는 대상이 있다면 아래 로직을 진행합니다.
		TArray<AStereoMixPlayerCharacter*> CatchableCharacters;
		if (GetCatchableCharacters(OverlapResults, CatchableCharacters))
		{
			// 커서 위치에 가장 가까운 캐릭터를 TargetCharacter에 담습니다.
			AStereoMixPlayerCharacter* TargetCharacter = GetClosestCharacterFromLocation(CatchableCharacters, InCursorLocation);
			if (ensure(TargetCharacter))
			{
				// 각 액터에게 필요한 태그를 GE를 통해 붙입니다.
				UStereoMixAbilitySystemComponent* SourceASC = GetStereoMixAbilitySystemComponentFromActorInfo();
				UStereoMixAbilitySystemComponent* TargetASC = Cast<UStereoMixAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetCharacter));
				if (ensure(SourceASC && TargetASC))
				{
					const FGameplayEffectContextHandle SourceGEContextHandle = SourceASC->MakeEffectContext();
					if (ensure(SourceGEContextHandle.IsValid() && AddCatchStateGE))
					{
						SourceASC->BP_ApplyGameplayEffectToSelf(AddCatchStateGE, 0.0f, SourceGEContextHandle);
					}

					const FGameplayEffectContextHandle TargetGEContextHandle = TargetASC->MakeEffectContext();
					if (ensure(TargetGEContextHandle.IsValid() && AddCaughtStateGE))
					{
						TargetASC->BP_ApplyGameplayEffectToSelf(AddCaughtStateGE, 0.0f, TargetGEContextHandle);
					}

					// 어태치하고 잡힌 대상은 잡히기 GA를 활성화합니다.
					AttachTargetCharacter(TargetCharacter);
					TargetASC->TryActivateAbilitiesByTag(FGameplayTagContainer(StereoMixTag::Ability::Caught));
					bSuccess = true;
				}
			}
		}
	}

	// 디버거
	const FColor Color = bSuccess ? FColor::Green : FColor::Red;
	DrawDebugSphere(GetWorld(), InStartLocation, Distance, 16, Color, false, 2.0f);
}

bool UStereoMixGameplayAbility_Catch::GetCatchableCharacters(const TArray<FOverlapResult>& InOverlapResults, TArray<AStereoMixPlayerCharacter*>& OutCatchableCharacters)
{
	OutCatchableCharacters.Empty();

	bool bIsCanCatchableCharacter = false;
	for (const auto& OverlapResult : InOverlapResults)
	{
		// 플레이어 캐릭터만 추려냅니다.
		AStereoMixPlayerCharacter* TargetCharacter = Cast<AStereoMixPlayerCharacter>(OverlapResult.GetActor());
		if (TargetCharacter)
		{
			// 태그를 기반으로 추려냅니다.
			const UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetCharacter);
			if (ensure(TargetASC))
			{
				// TODO: 여기서 팀이 다르면 걸러줘야합니다.

				if (TargetASC->HasMatchingGameplayTag(StereoMixTag::Character::State::Uncatchable))
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

AStereoMixPlayerCharacter* UStereoMixGameplayAbility_Catch::GetClosestCharacterFromLocation(const TArray<AStereoMixPlayerCharacter*>& InCharacters, const FVector& InLocation)
{
	AStereoMixPlayerCharacter* TargetCharacter = nullptr;
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

void UStereoMixGameplayAbility_Catch::AttachTargetCharacter(AStereoMixPlayerCharacter* InTargetCharacter) const
{
	const AStereoMixPlayerCharacter* SourceCharacter = GetStereoMixPlayerCharacterFromActorInfo();
	if (ensure(SourceCharacter))
	{
		// 잡기 전에 위치 보정 무시를 활성화합니다.
		InTargetCharacter->GetCharacterMovement()->bIgnoreClientMovementErrorChecksAndCorrection = true;

		// 어태치합니다. 디버깅을 위해 단언을 수행합니다.
		ensure(InTargetCharacter->AttachToComponent(SourceCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("CatchSocket")));
	}
}
