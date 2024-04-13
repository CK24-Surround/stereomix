// Fill out your copyright notice in the Description page of Project Settings.


#include "SMGameplayAbility_Catch.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "Characters/SMPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Utilities/SMCollision.h"
#include "Utilities/SMTags.h"

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
	ActivationBlockedTags = BlockedTags;
}

void USMGameplayAbility_Catch::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ASMPlayerCharacter* SourceCharacter = GetStereoMixPlayerCharacterFromActorInfo();
	if (!ensure(SourceCharacter))
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

		WaitGameplayEventTask->EventReceived.AddDynamic(this, &USMGameplayAbility_Catch::OnHoldAnimNotify);
		WaitGameplayEventTask->ReadyForActivation();
	}

	CommitAbility(Handle, ActorInfo, ActivationInfo);
}

void USMGameplayAbility_Catch::OnInterrupted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void USMGameplayAbility_Catch::OnComplete()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void USMGameplayAbility_Catch::OnHoldAnimNotify(FGameplayEventData Payload)
{
	ServerRPCRequestCatchProcess(StartLocation, TargetLocation);
}

void USMGameplayAbility_Catch::ServerRPCRequestCatchProcess_Implementation(const FVector_NetQuantize10& InStartLocation, const FVector_NetQuantize10& InCursorLocation)
{
	ASMPlayerCharacter* SourceCharacter = GetStereoMixPlayerCharacterFromActorInfo();
	if (!ensure(SourceCharacter))
	{
		return;
	}

	// 일정 범위내에 있는 캐릭터를 대상으로 충돌 검사를 수행합니다.
	TArray<FOverlapResult> OverlapResults;
	const float Distance = 300.0f;
	const FCollisionShape CollisionShape = FCollisionShape::MakeSphere(Distance);
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
				USMAbilitySystemComponent* SourceASC = GetStereoMixAbilitySystemComponentFromActorInfo();
				USMAbilitySystemComponent* TargetASC = Cast<USMAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetCharacter));
				if (ensure(SourceASC && TargetASC))
				{
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
				}
			}
		}
	}

	// 디버거
	const FColor Color = bSuccess ? FColor::Green : FColor::Red;
	DrawDebugSphere(GetWorld(), InStartLocation, Distance, 16, Color, false, 2.0f);
}

bool USMGameplayAbility_Catch::GetCatchableCharacters(const TArray<FOverlapResult>& InOverlapResults, TArray<ASMPlayerCharacter*>& OutCatchableCharacters)
{
	OutCatchableCharacters.Empty();

	bool bIsCanCatchableCharacter = false;
	for (const auto& OverlapResult : InOverlapResults)
	{
		// 플레이어 캐릭터만 추려냅니다.
		ASMPlayerCharacter* TargetCharacter = Cast<ASMPlayerCharacter>(OverlapResult.GetActor());
		if (TargetCharacter)
		{
			// 태그를 기반으로 추려냅니다. 일반적으로 TargetCharacter는 AStereoMixPlayerCharacter이기 때문에 TargetASC가 유효해야합니다.
			const UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetCharacter);
			if (ensure(TargetASC))
			{
				// TODO: 여기서 팀이 다르면 걸러줘야합니다.

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
	ASMPlayerCharacter* SourceCharacter = GetStereoMixPlayerCharacterFromActorInfo();
	if (ensure(SourceCharacter && InTargetCharacter))
	{
		// 어태치합니다. 디버깅을 위해 단언을 수행합니다. 어태치 후 상대 회전을 0으로 정렬해줍니다.
		if (InTargetCharacter->AttachToComponent(SourceCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("CatchSocket")))
		{
			// 충돌판정, 움직임을 잠급니다.
			InTargetCharacter->SetEnableCollision(false);
			InTargetCharacter->SetEnableMovement(false);

			// 타겟의 위치 보정 무시를 활성화합니다.
			UCharacterMovementComponent* TargetMovement = InTargetCharacter->GetCharacterMovement();
			if (ensure(TargetMovement))
			{
				TargetMovement->bIgnoreClientMovementErrorChecksAndCorrection = true;
			}

			InTargetCharacter->MulticastRPCResetRelativeRotation();

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
