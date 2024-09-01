// Copyright Surround, Inc. All Rights Reserved.


#include "SMGA_Neutralize.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "Characters/Player/SMPlayerCharacterBase.h"
#include "Data/Character/SMPlayerCharacterDataAsset.h"
#include "FunctionLibraries/SMHoldInteractionBlueprintLibrary.h"
#include "HoldInteraction/SMHIC_Character.h"

USMGA_Neutralize::USMGA_Neutralize()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
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
	if (!SourceCharacter)
	{
		EndAbilityByCancel();
		return;
	}

	UAbilitySystemComponent* SourceASC = GetASC<UAbilitySystemComponent>();
	if (!ensureAlways(SourceASC))
	{
		EndAbilityByCancel();
		return;
	}

	const USMPlayerCharacterDataAsset* SourceCharacterDataAsset = SourceCharacter->GetDataAsset();
	const ESMTeam SourceTeam = SourceCharacter->GetTeam();
	CachedNeutralizeMontage = SourceCharacterDataAsset->NeutralizeMontage[SourceTeam];

	SourceASC->PlayMontage(this, ActivationInfo, CachedNeutralizeMontage, 1.0f);
	ClientRPCPlayMontage(CachedNeutralizeMontage);

	SourceCharacter->SetUseControllerRotation(false);

	if (SourceASC->HasMatchingGameplayTag(SMTags::Character::State::Hold))
	{
		Release();
	}

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &USMGA_Neutralize::OnNeutralizeTimeEnd, NeutralizedTime, false);

	K2_CommitAbility();

	// 캐릭터 상태 위젯을 숨깁니다.
	SourceCharacter->SetCharacterStateVisibility(false);

	// 다른 클라이언트들에게 자신을 타겟하는 스크린 인디케이터를 활성화하도록 합니다.
	SourceCharacter->MulticastRPCAddScreenIndicatorToSelf(SourceCharacter);

	// 무력화 이펙트를 재생합니다.
	FGameplayCueParameters GCParams;
	GCParams.Instigator = SourceCharacter->GetLastAttackInstigator();
	GCParams.TargetAttachComponent = SourceCharacter->GetMesh();
	SourceASC->AddGameplayCue(SMTags::GameplayCue::Neutralize, GCParams);
}

void USMGA_Neutralize::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	ASMPlayerCharacterBase* SourceCharacter = GetAvatarActor<ASMPlayerCharacterBase>();
	if (!SourceCharacter)
	{
		Super::EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	USMAbilitySystemComponent* SourceASC = GetASC<USMAbilitySystemComponent>();
	if (!SourceASC)
	{
		Super::EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (!bWasCancelled)
	{
		// 면역상태로 진입합니다. 딜레이로 인해 데미지를 받을 수도 있으니 GA실행 전에 먼저 면역태그를 추가합니다.
		// SourceASC->AddTag(SMTags::Character::State::Immune);
		// SourceASC->TryActivateAbilitiesByTag(FGameplayTagContainer(SMTags::Ability::Immune));
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USMGA_Neutralize::OnNeutralizeTimeEnd()
{
	ASMPlayerCharacterBase* SourceCharacter = GetAvatarActor<ASMPlayerCharacterBase>();
	if (!SourceCharacter)
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

	// 잡을 수 없는 상태임을 명시하는 태그를 부착합니다. 시간 초과 후 추가 잡기를 방지합니다.
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
		NeutralizeExit();
	}
}

void USMGA_Neutralize::WaitUntilBuzzerBeaterEnd()
{
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

	// 만약 어떤 이유로 자신을 잡고 있는 캐릭터가 제거된 경우의 예외처리입니다.
	if (!SourceHIC->GetActorHoldingMe())
	{
		SourceASC->RemoveTag(SMTags::Character::State::Smashed);
		// 즉시 잡기 탈출 로직을 호출합니다.
		// NeutralizeExit();
		return;
	}

	// 스매시 이벤트를 기다립니다.
	NET_LOG(GetAvatarActorFromActorInfo(), Log, TEXT("버저 비터 진입"));
	UAbilityTask_WaitGameplayEvent* WaitEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, SMTags::Event::Character::BuzzerBeaterEnd);
	if (!ensureAlways(WaitEventTask))
	{
		EndAbilityByCancel();
		return;
	}
	WaitEventTask->EventReceived.AddDynamic(this, &ThisClass::OnBuzzerBeaterEnded);
	WaitEventTask->ReadyForActivation();
}

void USMGA_Neutralize::OnBuzzerBeaterEnded(FGameplayEventData Payload)
{
	NeutralizeExit();
}

void USMGA_Neutralize::HoldedStateExit()
{
	USMHIC_Character* SourceHIC = GetHIC<USMHIC_Character>();
	if (!ensureAlways(SourceHIC))
	{
		EndAbilityByCancel();
		return;
	}

	// 잡히기 상태에서 탈출합니다.
	SourceHIC->OnHoldedReleased(SourceHIC->GetActorHoldingMe(), true);

	NeutralizeExit();
}

void USMGA_Neutralize::NeutralizeExit()
{
	ASMPlayerCharacterBase* SourceCharacter = GetAvatarActor<ASMPlayerCharacterBase>();
	if (!SourceCharacter)
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

	UAnimInstance* SourceAnimInstance = CurrentActorInfo->GetAnimInstance();
	if (!ensureAlways(SourceAnimInstance))
	{
		EndAbilityByCancel();
		return;
	}

	const USMPlayerCharacterDataAsset* SourceCharacterDataAsset = SourceCharacter->GetDataAsset();
	if (!SourceCharacterDataAsset)
	{
		EndAbilityByCancel();
		return;
	}

	// 노이즈 브레이크를 1회라도 당해 넘어진 상태이거나, 잡혀있는 상태인 경우 무력화 종료 애니메이션이 달라져야하는데 이를 위해 현재 실행중인 애니메이션의 End 섹션으로 점프시키는 코드입니다.
	// 해당 애니메이션을 무한루프하고 있는 상태이므로 가능합니다.
	UAnimMontage* EndMontage = SourceAnimInstance->GetCurrentActiveMontage();
	const FName EndSectionName = TEXT("End");
	ClientRPCPlayMontage(EndMontage, 1.0f, EndSectionName);
	UAbilityTask_PlayMontageAndWait* MontageWaitTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("NeutralizeEnd"), EndMontage, 1.0f, EndSectionName);
	if (!ensureAlways(MontageWaitTask))
	{
		EndAbilityByCancel();
		return;
	}
	MontageWaitTask->OnCompleted.AddDynamic(this, &ThisClass::NeutralizeExitMontageEnded);
	MontageWaitTask->ReadyForActivation();

	NewtralizeEndEffect();
}

void USMGA_Neutralize::NeutralizeExitMontageEnded()
{
	ASMPlayerCharacterBase* SourceCharacter = GetAvatarActor<ASMPlayerCharacterBase>();
	if (!SourceCharacter)
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

	// 스턴이 완전히 종료되었기에 Uncatchable 태그를 제거하고 자신을 잡았던 캐릭터 리스트를 초기화합니다.
	SourceASC->RemoveTag(SMTags::Character::State::Unholdable);
	SourceHIC->EmptyHoldedMeCharacterList();

	// 스턴 종료 시 적용해야할 GE들을 적용합니다.
	for (const auto& StunEndedGE : StunEndedGEs)
	{
		if (ensure(StunEndedGE))
		{
			BP_ApplyGameplayEffectToOwner(StunEndedGE);
		}
	}

	// 컨트롤 로테이션을 따라가도록 복구해줍니다.
	SourceCharacter->SetUseControllerRotation(true);

	// 캐릭터 상태 위젯을 다시 보이게합니다.
	SourceCharacter->SetCharacterStateVisibility(true);

	K2_EndAbility();
}

void USMGA_Neutralize::Release()
{
	ASMPlayerCharacterBase* SourceCharacter = GetAvatarActor<ASMPlayerCharacterBase>();
	if (!ensureAlways(SourceCharacter))
	{
		return;
	}

	UAbilitySystemComponent* SourceASC = GetASC<UAbilitySystemComponent>();
	if (!ensureAlways(SourceASC))
	{
		return;
	}

	USMHIC_Character* SourceHIC = GetHIC<USMHIC_Character>();
	if (!ensureAlways(SourceHIC))
	{
		return;
	}

	AActor* TargetActor = SourceHIC->GetActorIAmHolding();
	if (!ensureAlways(TargetActor))
	{
		return;
	}

	USMHoldInteractionComponent* TargetHIC = USMHoldInteractionBlueprintLibrary::GetHoldInteractionComponent(TargetActor);
	if (!ensureAlways(TargetHIC))
	{
		return;
	}

	TargetHIC->OnHoldedReleased(SourceCharacter, false);
	SourceHIC->SetActorIAmHolding(nullptr);
}

void USMGA_Neutralize::NewtralizeEndEffect()
{
	UAbilitySystemComponent* SourceASC = GetASC<UAbilitySystemComponent>();
	if (ensureAlways(SourceASC))
	{
		// 무력화 종료 이펙트를 재생합니다.
		SourceASC->RemoveGameplayCue(SMTags::GameplayCue::Neutralize);
	}
}
