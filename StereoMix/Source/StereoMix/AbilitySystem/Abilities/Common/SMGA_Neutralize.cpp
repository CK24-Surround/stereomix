// Copyright Surround, Inc. All Rights Reserved.


#include "SMGA_Neutralize.h"

#include "Abilities/Tasks/AbilityTask_NetworkSyncPoint.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "AbilitySystem/Abilities/NoiseBreak/Indicator/SMGA_NoiseBreakIndicator.h"
#include "Actors/Character/Player/SMPlayerCharacterBase.h"
#include "Components/Character/SMHIC_Character.h"
#include "Data/DataAsset/Character/SMPlayerCharacterDataAsset.h"
#include "FunctionLibraries/SMHoldInteractionBlueprintLibrary.h"
#include "FunctionLibraries/SMScoreFunctionLibrary.h"

USMGA_Neutralize::USMGA_Neutralize()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;

	ActivationOwnedTags = FGameplayTagContainer(SMTags::Character::State::Common::Neutralized);

	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = SMTags::Event::Character::Neutralize;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);

	NoiseBreakedTags.AddTag(SMTags::Character::State::Common::NoiseBreaked);
}

void USMGA_Neutralize::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ASMPlayerCharacterBase* SourceCharacter = GetCharacter();
	const USMAbilitySystemComponent* SourceASC = GetASC();
	USMHIC_Character* SourceHIC = GetHIC();
	if (!SourceCharacter || !SourceASC || !SourceHIC)
	{
		EndAbilityByCancel();
		return;
	}

	NET_LOG(GetAvatarActor(), Log, TEXT("%s가 무력화 됨"), *GetNameSafe(GetAvatarActor()));

	K2_CommitAbility();

	// 재생 중인 모든 애니메이션을 종료합니다.
	if (UAnimInstance* SourceAnimInstance = ActorInfo->GetAnimInstance())
	{
		SourceAnimInstance->StopAllMontages(0.0f);
	}

	if (K2_HasAuthority())
	{
		USMScoreFunctionLibrary::RecordKillDeathCount(SourceCharacter->GetLastAttacker(), SourceCharacter);

		// 무력화 시간동안 기다립니다.
		WaitTask = UAbilityTask_WaitDelay::WaitDelay(this, NeutralizedTime);
		WaitTask->OnFinish.AddDynamic(this, &ThisClass::OnNeutralizeTimeEnded);
		WaitTask->ReadyForActivation();

		// 동시에 최소 무력화 시간 타이머도 추가합니다.
		MinimalWaitTask = UAbilityTask_WaitDelay::WaitDelay(this, MinimalNeutralizedTime);
		MinimalWaitTask->OnFinish.AddDynamic(this, &ThisClass::OnMinimalNeutralizeTimeEnded);
		MinimalWaitTask->ReadyForActivation();

		// 잡힐 경우 최소 무력화 시간 타이머가 리셋됩니다.
		SourceHIC->OnHeldStateEntry.AddUObject(this, &ThisClass::MinimalNeutralizeTimerReset);

		// 노이즈 브레이크 당해도 최소 무력화 시간 타이머가 리셋됩니다.
		WaitNoiseBreakEndWaitTaskForMinimalTimer = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, SMTags::Event::Character::BuzzerBeaterEnd);
		WaitNoiseBreakEndWaitTaskForMinimalTimer->EventReceived.AddDynamic(this, &ThisClass::OnNoiseBreakEndedForMinimalTimer);
		WaitNoiseBreakEndWaitTaskForMinimalTimer->ReadyForActivation();

		// 무언가를 잡고 있다면 잡은 대상을 놓습니다.
		if (SourceASC->HasMatchingGameplayTag(SMTags::Character::State::Common::Hold))
		{
			AActor* TargetActor = SourceHIC->GetActorIAmHolding();
			if (USMHoldInteractionComponent* TargetHIC = USMHoldInteractionBlueprintLibrary::GetHoldInteractionComponent(TargetActor))
			{
				TargetHIC->OnReleasedFromHold(SourceCharacter);
			}

			SourceHIC->SetActorIAmHolding(nullptr);
		}

		// 무력화 후 어떤 액션을 당해도 커서를 바라보지 않도록 합니다.
		// 만약 잠그지 않는다면 노이즈브레이크를 당해도 마지막으로 입력된 커서의 방향을 바라보게 됩니다.
		SourceCharacter->ServerSetUseControllerRotation(false);

		// 캐릭터 상태 위젯을 숨깁니다.
		SourceCharacter->MulticastSetCharacterStateVisibility(false);

		// 다른 클라이언트들에게 자신을 타겟하는 스크린 인디케이터를 활성화하도록 합니다.
		SourceCharacter->MulticastAddScreenIndicatorToSelf(SourceCharacter);

		// 캐릭터를 노트 상태로 변경합니다.
		SourceCharacter->ServerSetNoteState(true);

		FGameplayCueParameters GCParams;
		GCParams.SourceObject = SourceCharacter;
		SourceASC->AddGC(SourceCharacter, SMTags::GameplayCue::Common::Neutralize, GCParams);
	}
	else if (IsLocallyControlled()) // 데디 서버를 위한 예외처리입니다.
	{
		NeutralizeExitSyncPoint();
	}
}

void USMGA_Neutralize::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (!bWasCancelled)
	{
		if (K2_HasAuthority())
		{
			ASMPlayerCharacterBase* SourceCharacter = GetCharacter();
			USMAbilitySystemComponent* SourceASC = GetASC();
			if (SourceCharacter)
			{
				// 컨트롤 로테이션을 따라가도록 복구해줍니다.
				SourceCharacter->ServerSetUseControllerRotation(true);

				// 캐릭터 상태 위젯을 다시 보이게합니다.
				SourceCharacter->MulticastSetCharacterStateVisibility(true);

				// 캐릭터를 노트 상태로 변경합니다.
				SourceCharacter->ServerSetNoteState(false);

				if (SourceASC)
				{
					FGameplayCueParameters GCParams;
					GCParams.SourceObject = SourceCharacter;
					SourceASC->RemoveGC(SourceCharacter, SMTags::GameplayCue::Common::Neutralize, GCParams);
				}
			}

			if (USMHIC_Character* SourceHIC = GetHIC<USMHIC_Character>())
			{
				SourceHIC->EmptyHeldMeCharacterList(); // 무력화가 종료되었기에 자신을 잡았던 캐릭터 리스트를 초기화합니다.
				SourceHIC->OnHeldStateEntry.RemoveAll(this);
			}

			// 스턴 종료 시 적용해야할 GE들을 적용합니다.
			for (const auto& StunEndedGE : StunEndedGEs)
			{
				if (ensure(StunEndedGE))
				{
					BP_ApplyGameplayEffectToOwner(StunEndedGE);
				}
			}

			if (SourceASC)
			{
				// 면역상태로 진입합니다. 딜레이로 인해 데미지를 받을 수도 있으니 GA실행 전에 먼저 면역태그를 추가합니다.
				SourceASC->AddTag(SMTags::Character::State::Common::Immune);
				SourceASC->TryActivateAbilitiesByTag(FGameplayTagContainer(SMTags::Ability::Immune));

				SourceASC->RemoveTag(SMTags::Character::State::Common::NonHoldable);
			}
		}
	}

	NET_LOG(GetAvatarActorFromActorInfo(), Log, TEXT("%s의 무력화 종료"), *GetNameSafe(GetAvatarActor()));

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USMGA_Neutralize::OnMinimalNeutralizeTimeEnded()
{
	USMAbilitySystemComponent* SourceASC = GetASC();
	const USMHIC_Character* SourceHIC = GetHIC();
	if (!SourceASC || !SourceHIC)
	{
		EndAbilityByCancel();
		return;
	}

	if (SourceASC->HasAnyMatchingGameplayTags(NoiseBreakedTags)) // 노이즈 브레이크 당하는 중이라면 노이즈 브레이크 종료 이벤트에 최소 타이머 리셋을 맡깁니다. 따라서 여기서 처리하지 않습니다.
	{
		if (!SourceHIC->GetActorHoldingMe()) // 게임 종료 등의 이유로 자신을 잡고 있는 캐릭터가 노이즈 브레이크하다가 사라진 경우 예외처리입니다.
		{
			SourceASC->RemoveTag(SMTags::Character::State::Common::NoiseBreaked);
		}
	}
	else
	{
		// 무력화 조기 종료를 위한 상태로 진입합니다.
		if (WaitTask)
		{
			WaitTask->EndTask();
		}

		NET_LOG(GetAvatarActor(), Log, TEXT("%s가 무력화 조기 종료 상태로 진입"), *GetNameSafe(GetAvatarActor()));
		StartNeutralizeExit();
	}
}

void USMGA_Neutralize::MinimalNeutralizeTimerReset()
{
	if (MinimalWaitTask)
	{
		MinimalWaitTask->EndTask();
	}

	MinimalWaitTask = UAbilityTask_WaitDelay::WaitDelay(this, MinimalNeutralizedTime);
	MinimalWaitTask->OnFinish.AddDynamic(this, &ThisClass::OnMinimalNeutralizeTimeEnded);
	MinimalWaitTask->ReadyForActivation();
}

void USMGA_Neutralize::OnNoiseBreakEndedForMinimalTimer(FGameplayEventData Payload)
{
	// 최소 타이머와 관련된 태스크들을 정리해줍니다.
	if (MinimalWaitTask)
	{
		MinimalWaitTask->EndTask();
	}
	if (WaitNoiseBreakEndWaitTaskForMinimalTimer)
	{
		WaitNoiseBreakEndWaitTaskForMinimalTimer->EndTask();
	}

	MinimalNeutralizeTimerReset();
}

void USMGA_Neutralize::OnNeutralizeTimeEnded()
{
	NET_LOG(GetAvatarActor(), Log, TEXT("%s가 무력화 종료 상태로 진입"), *GetNameSafe(GetAvatarActor()));
	StartNeutralizeExit();
}

void USMGA_Neutralize::StartNeutralizeExit()
{
	USMAbilitySystemComponent* SourceASC = GetASC();
	if (!SourceASC)
	{
		EndAbilityByCancel();
		return;
	}

	// 잡을 수 없는 상태임을 명시하는 태그를 부착합니다. 시간 초과 이후의 잡기를 방지합니다.
	SourceASC->AddTag(SMTags::Character::State::Common::NonHoldable);

	if (SourceASC->HasAnyMatchingGameplayTags(NoiseBreakedTags)) // 노이즈 브레이크 당하는 중이라면 버저비터 대기로 진입합니다.
	{
		WaitUntilBuzzerBeaterEnd();
	}
	else
	{
		PrepareNeutralizeEnd();
	}
}

void USMGA_Neutralize::WaitUntilBuzzerBeaterEnd()
{
	USMAbilitySystemComponent* SourceASC = GetASC();
	const USMHIC_Character* SourceHIC = GetHIC();
	if (!SourceASC || !SourceHIC)
	{
		EndAbilityByCancel();
		return;
	}

	// 게임 종료 등의 이유로 자신을 잡고 있는 캐릭터가 노이즈 브레이크하다가 사라진 경우 예외처리입니다.
	if (!SourceHIC->GetActorHoldingMe())
	{
		// 즉시 잡기 탈출 로직을 호출합니다.
		SourceASC->RemoveTag(SMTags::Character::State::Common::NoiseBreaked);
		NeutralizeExitSyncPoint();
		return;
	}

	NET_LOG(GetAvatarActorFromActorInfo(), Warning, TEXT("%s가 버저 비터 종료 대기 상태 진입"), *GetNameSafe(GetAvatarActor()));

	// 버저비터 종료 이벤트를 기다립니다.
	UAbilityTask_WaitGameplayEvent* WaitEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, SMTags::Event::Character::BuzzerBeaterEnd);
	WaitEventTask->EventReceived.AddDynamic(this, &ThisClass::OnBuzzerBeaterEnded);
	WaitEventTask->ReadyForActivation();
}

void USMGA_Neutralize::OnBuzzerBeaterEnded(FGameplayEventData Payload)
{
	NET_LOG(GetAvatarActorFromActorInfo(), Log, TEXT("%s의 버저 비터 종료"), *GetNameSafe(GetAvatarActor()));

	// 노이즈 브레이크가 끝나면 잡기 상태도 풀려있어 PrepareNeutralizeEnd를 호출할 필요가 없습니다.
	NeutralizeExitSyncPoint();
}

void USMGA_Neutralize::PrepareNeutralizeEnd()
{
	const UAbilitySystemComponent* SourceASC = GetASC();
	USMHIC_Character* SourceHIC = GetHIC();
	if (!SourceASC || !SourceHIC)
	{
		EndAbilityByCancel();
		return;
	}

	// 만약 잡혀 있다면 잡히기 상태에서 탈출합니다.
	if (SourceASC->HasMatchingGameplayTag(SMTags::Character::State::Common::Held))
	{
		SourceHIC->OnReleasedFromHold(SourceHIC->GetActorHoldingMe());
	}

	NeutralizeExitSyncPoint();
}

void USMGA_Neutralize::NeutralizeExitSyncPoint()
{
	UAbilityTask_NetworkSyncPoint* SyncTask = UAbilityTask_NetworkSyncPoint::WaitNetSync(this, EAbilityTaskNetSyncType::BothWait);
	SyncTask->OnSync.AddDynamic(this, &ThisClass::NeutralizeEnd);
	SyncTask->ReadyForActivation();
}

void USMGA_Neutralize::NeutralizeEnd()
{
	ASMPlayerCharacterBase* SourceCharacter = GetCharacter();
	const USMAbilitySystemComponent* SourceASC = GetASC();
	const USMPlayerCharacterDataAsset* SourceCharacterDataAsset = GetDataAsset();
	const UAnimInstance* SourceAnimInstance = CurrentActorInfo ? CurrentActorInfo->GetAnimInstance() : nullptr;
	if (!SourceCharacter || !SourceASC || !SourceCharacterDataAsset || !SourceAnimInstance)
	{
		EndAbilityByCancel();
		return;
	}

	// 다른 클라이언트들에게 자신을 타겟하는 스크린 인디케이터를 제거하도록 합니다.
	if (K2_HasAuthority())
	{
		SourceCharacter->MulticastRemoveScreenIndicatorToSelf(SourceCharacter);
	}

	K2_EndAbility();
}
