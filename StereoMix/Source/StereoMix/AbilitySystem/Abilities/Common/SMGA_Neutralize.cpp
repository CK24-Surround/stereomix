// Copyright Surround, Inc. All Rights Reserved.


#include "SMGA_Neutralize.h"

#include "Abilities/Tasks/AbilityTask_NetworkSyncPoint.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "AbilitySystem/Abilities/NoiseBreak/Indicator/SMGA_NoiseBreakIndicator.h"
#include "Characters/Player/SMPlayerCharacterBase.h"
#include "Data/Character/SMPlayerCharacterDataAsset.h"
#include "FunctionLibraries/SMHoldInteractionBlueprintLibrary.h"
#include "HoldInteraction/SMHIC_Character.h"

USMGA_Neutralize::USMGA_Neutralize()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;

	ActivationOwnedTags = FGameplayTagContainer(SMTags::Character::State::Neutralize);

	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = SMTags::Event::Character::Neutralize;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);

	NoiseBreakedTags.AddTag(SMTags::Character::State::NoiseBreaked);
}

void USMGA_Neutralize::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ASMPlayerCharacterBase* SourceCharacter = GetAvatarActor<ASMPlayerCharacterBase>();
	UAbilitySystemComponent* SourceASC = GetASC<UAbilitySystemComponent>();
	const USMPlayerCharacterDataAsset* SourceDataAsset = SourceCharacter ? SourceCharacter->GetDataAsset() : nullptr;
	if (!SourceCharacter || !SourceASC || !SourceDataAsset)
	{
		EndAbilityByCancel();
		return;
	}

	K2_CommitAbility();

	// 무력화 몽타주를 재생합니다.
	const FName MontageTaskName = TEXT("MontageTask");
	const ESMTeam SourceTeam = SourceCharacter->GetTeam();
	CachedNeutralizeMontage = SourceDataAsset->NeutralizeMontage[SourceTeam];
	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, MontageTaskName, CachedNeutralizeMontage);
	MontageTask->ReadyForActivation();

	// 노이즈 브레이크 인디케이터가 활성화 되어있다면 제거합니다.
	FGameplayAbilitySpec* GASpec = SourceASC->FindAbilitySpecFromClass(USMGA_NoiseBreakIndicator::StaticClass());
	if (GASpec)
	{
		SourceASC->CancelAbilityHandle(GASpec->Handle);
	}

	if (K2_HasAuthority())
	{
		// 무력화 시간동안 기다립니다.
		UAbilityTask_WaitDelay* WaitTask = UAbilityTask_WaitDelay::WaitDelay(this, NeutralizedTime);
		WaitTask->OnFinish.AddDynamic(this, &ThisClass::OnNeutralizeTimeEnded);
		WaitTask->ReadyForActivation();

		// 무언가를 잡고 있다면 잡은 대상을 놓습니다.
		if (SourceASC->HasMatchingGameplayTag(SMTags::Character::State::Hold))
		{
			Release();
		}

		// 무력화 후 어떤 액션을 당해도 커서를 바라보지 않도록 합니다.
		// 만약 잠그지 않는다면 노이즈브레이크를 당해도 마지막으로 입력된 커서의 방향을 바라보게 됩니다.
		SourceCharacter->SetUseControllerRotation(false);

		// 캐릭터 상태 위젯을 숨깁니다.
		SourceCharacter->MulticastSetCharacterStateVisibility(false);

		// 다른 클라이언트들에게 자신을 타겟하는 스크린 인디케이터를 활성화하도록 합니다.
		SourceCharacter->MulticastRPCAddScreenIndicatorToSelf(SourceCharacter);
	}
	else if (IsLocallyControlled())
	{
		NeutralizeExitSyncPoint();
	}

	// 무력화 이펙트를 재생합니다.
	// FGameplayCueParameters GCParams;
	// GCParams.Instigator = SourceCharacter->GetLastAttackInstigator();
	// GCParams.TargetAttachComponent = SourceCharacter->GetMesh();
	// SourceASC->AddGameplayCue(SMTags::GameplayCue::Neutralize, GCParams);
}

void USMGA_Neutralize::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (!bWasCancelled)
	{
		if (K2_HasAuthority())
		{
			ASMPlayerCharacterBase* SourceCharacter = GetAvatarActor<ASMPlayerCharacterBase>();
			if (SourceCharacter)
			{
				// 컨트롤 로테이션을 따라가도록 복구해줍니다.
				SourceCharacter->SetUseControllerRotation(true);

				// 캐릭터 상태 위젯을 다시 보이게합니다.
				SourceCharacter->MulticastSetCharacterStateVisibility(true);
			}

			USMHIC_Character* SourceHIC = GetHIC<USMHIC_Character>();
			if (SourceHIC)
			{
				// 무력화가 종료되었기에 자신을 잡았던 캐릭터 리스트를 초기화합니다.
				SourceHIC->EmptyHoldedMeCharacterList();
			}

			// 스턴 종료 시 적용해야할 GE들을 적용합니다.
			for (const auto& StunEndedGE : StunEndedGEs)
			{
				if (ensure(StunEndedGE))
				{
					BP_ApplyGameplayEffectToOwner(StunEndedGE);
				}
			}

			USMAbilitySystemComponent* SourceASC = GetASC<USMAbilitySystemComponent>();
			if (SourceASC)
			{
				// 면역상태로 진입합니다. 딜레이로 인해 데미지를 받을 수도 있으니 GA실행 전에 먼저 면역태그를 추가합니다.
				SourceASC->AddTag(SMTags::Character::State::Immune);
				SourceASC->TryActivateAbilitiesByTag(FGameplayTagContainer(SMTags::Ability::Immune));

				SourceASC->RemoveTag(SMTags::Character::State::Unholdable);
			}
		}
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USMGA_Neutralize::OnNeutralizeTimeEnded()
{
	ASMPlayerCharacterBase* SourceCharacter = GetAvatarActor<ASMPlayerCharacterBase>();
	USMAbilitySystemComponent* SourceASC = GetASC<USMAbilitySystemComponent>();
	if (!SourceCharacter || !SourceASC)
	{
		EndAbilityByCancel();
		return;
	}

	// 잡을 수 없는 상태임을 명시하는 태그를 부착합니다. 시간 초과 이후의 잡기를 방지합니다.
	SourceASC->AddTag(SMTags::Character::State::Unholdable);

	// 다른 클라이언트들에게 자신을 타겟하는 스크린 인디케이터를 제거하도록 합니다.
	SourceCharacter->MulticastRPCRemoveScreenIndicatorToSelf(SourceCharacter);

	// 노이즈 브레이크 당하는 중의 처리입니다.
	if (SourceASC->HasAnyMatchingGameplayTags(NoiseBreakedTags))
	{
		WaitUntilBuzzerBeaterEnd();
	}
	// 잡힌 상태인 경우의 처리입니다.
	else if (SourceASC->HasMatchingGameplayTag(SMTags::Character::State::Holded))
	{
		HoldedStateExit();
	}
	// 기절 상태인 경우의 처리입니다.
	else
	{
		NeutralizeExitSyncPoint();
	}
}

void USMGA_Neutralize::WaitUntilBuzzerBeaterEnd()
{
	USMAbilitySystemComponent* SourceASC = GetASC<USMAbilitySystemComponent>();
	USMHIC_Character* SourceHIC = GetHIC<USMHIC_Character>();
	if (!SourceASC || !SourceHIC)
	{
		EndAbilityByCancel();
		return;
	}

	// 게임 종료 등의 이유로 자신을 잡고 있는 캐릭터가 노이즈 브레이크하다가 사라진 경우 예외처리입니다.
	if (!SourceHIC->GetActorHoldingMe())
	{
		// 즉시 잡기 탈출 로직을 호출합니다.
		SourceASC->RemoveTag(SMTags::Character::State::NoiseBreaked);
		NeutralizeExitSyncPoint();
		return;
	}

	// 버저비터 종료 이벤트를 기다립니다.
	NET_LOG(GetAvatarActorFromActorInfo(), Log, TEXT("버저 비터 진입"));
	UAbilityTask_WaitGameplayEvent* WaitEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, SMTags::Event::Character::BuzzerBeaterEnd);
	WaitEventTask->EventReceived.AddDynamic(this, &ThisClass::OnBuzzerBeaterEnded);
	WaitEventTask->ReadyForActivation();
}

void USMGA_Neutralize::OnBuzzerBeaterEnded(FGameplayEventData Payload)
{
	NeutralizeExitSyncPoint();
}

void USMGA_Neutralize::HoldedStateExit()
{
	USMHIC_Character* SourceHIC = GetHIC<USMHIC_Character>();
	if (!SourceHIC)
	{
		EndAbilityByCancel();
		return;
	}

	// 잡히기 상태에서 탈출합니다.
	SourceHIC->OnHoldedReleased(SourceHIC->GetActorHoldingMe());
	NeutralizeExitSyncPoint();
}

void USMGA_Neutralize::NeutralizeExitSyncPoint()
{
	UAbilityTask_NetworkSyncPoint* SyncTask = UAbilityTask_NetworkSyncPoint::WaitNetSync(this, EAbilityTaskNetSyncType::BothWait);
	SyncTask->OnSync.AddDynamic(this, &ThisClass::NeutralizeExit);
	SyncTask->ReadyForActivation();
}

void USMGA_Neutralize::NeutralizeExit()
{
	ASMPlayerCharacterBase* SourceCharacter = GetAvatarActor<ASMPlayerCharacterBase>();
	USMAbilitySystemComponent* SourceASC = GetASC<USMAbilitySystemComponent>();
	const USMPlayerCharacterDataAsset* SourceCharacterDataAsset = GetDataAsset();
	UAnimInstance* SourceAnimInstance = CurrentActorInfo ? CurrentActorInfo->GetAnimInstance() : nullptr;
	if (!SourceCharacter || !SourceASC || !SourceCharacterDataAsset || !SourceAnimInstance)
	{
		EndAbilityByCancel();
		return;
	}

	// 노이즈 브레이크를 1회라도 당해 넘어진 상태이거나, 잡혀있는 상태인 경우 무력화 종료 애니메이션이 달라져야하는데 이를 위해 현재 실행중인 애니메이션의 End 섹션으로 점프시키는 코드입니다. 해당 애니메이션을 무한루프하고 있는 상태이므로 가능합니다.
	UAnimMontage* EndMontage = SourceAnimInstance->GetCurrentActiveMontage();
	NET_LOG(GetAvatarActor(), Log, TEXT("무력화 상태에 사용되고 있는 몽타주: %s"), *EndMontage->GetName());
	const FName EndMontageTaskName = TEXT("EndMontageTask");
	const FName SectionName = TEXT("End");
	UAbilityTask_PlayMontageAndWait* MontageWaitTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, EndMontageTaskName, EndMontage, 1.0f, SectionName);
	MontageWaitTask->OnCompleted.AddDynamic(this, &ThisClass::K2_EndAbility);
	MontageWaitTask->ReadyForActivation();

	// 무력화 종료 이펙트를 재생합니다.
	// SourceASC->RemoveGameplayCue(SMTags::GameplayCue::Neutralize);
}

void USMGA_Neutralize::Release()
{
	ASMPlayerCharacterBase* SourceCharacter = GetAvatarActor<ASMPlayerCharacterBase>();
	UAbilitySystemComponent* SourceASC = GetASC<UAbilitySystemComponent>();
	USMHIC_Character* SourceHIC = GetHIC<USMHIC_Character>();
	if (!SourceCharacter || !SourceASC || !SourceHIC)
	{
		return;
	}

	AActor* TargetActor = SourceHIC->GetActorIAmHolding();
	USMHoldInteractionComponent* TargetHIC = USMHoldInteractionBlueprintLibrary::GetHoldInteractionComponent(TargetActor);
	if (TargetHIC)
	{
		TargetHIC->OnHoldedReleased(SourceCharacter);
	}

	SourceHIC->SetActorIAmHolding(nullptr);
}
