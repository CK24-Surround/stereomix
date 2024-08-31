// Copyright Surround, Inc. All Rights Reserved.


#include "SMGA_Neutralize.h"

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

	NoiseBreakedTags.AddTag(SMTags::Character::State::BassNoiseBreaked);
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
	CachedNeutralizeMontage = SourceCharacterDataAsset->NeutralizedMontage[SourceTeam];

	SourceASC->PlayMontage(this, ActivationInfo, CachedNeutralizeMontage, 1.0f);
	ClientRPCPlayMontage(CachedNeutralizeMontage);

	SourceCharacter->SetUseControllerRotation(false);

	if (SourceASC->HasMatchingGameplayTag(SMTags::Character::State::Catch))
	{
		Release();
	}

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &USMGA_Neutralize::OnNeutralizeEnd, NeutralizedTime, false);

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
		SourceASC->AddTag(SMTags::Character::State::Immune);
		SourceASC->TryActivateAbilitiesByTag(FGameplayTagContainer(SMTags::Ability::Immune));
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
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

void USMGA_Neutralize::OnNeutralizeEnd()
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

	// 잡을 수 없는 태그를 부착합니다. 추가 잡기를 방지합니다.
	SourceASC->AddTag(SMTags::Character::State::Uncatchable);

	// 다른 클라이언트들에게 자신을 타겟하는 스크린 인디케이터를 제거하도록 합니다.
	SourceCharacter->MulticastRPCRemoveScreenIndicatorToSelf(SourceCharacter);

	// 노이즈 브레이크 당하는 중의 처리입니다.
	if (SourceASC->HasAnyMatchingGameplayTags(NoiseBreakedTags))
	{
		// ProcessBuzzerBeaterSmashed();
	}
	// 잡힌 상태인 경우의 처리입니다.
	else if (SourceASC->HasMatchingGameplayTag(SMTags::Character::State::Holded))
	{
		// ProcessCaughtExit();
	}
	// 기절 상태인 경우의 처리입니다.
	else
	{
		ResetNeutralizeState();
	}
}

void USMGA_Neutralize::ResetNeutralizeState()
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
}
