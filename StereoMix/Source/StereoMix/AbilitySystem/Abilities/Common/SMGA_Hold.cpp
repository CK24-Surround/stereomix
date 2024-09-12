// Copyright Surround, Inc. All Rights Reserved.


#include "SMGA_Hold.h"

#include "Abilities/Tasks/AbilityTask_NetworkSyncPoint.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "AbilitySystem/Task/SMAT_WaitHoldResult.h"
#include "Characters/Player/SMPlayerCharacterBase.h"
#include "Data/Character/SMPlayerCharacterDataAsset.h"
#include "HoldInteraction/SMHIC_Character.h"

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

	// 언제든 다른 상태로 인해 끊길 수 있는 애니메이션이기에 bAllowInterruptAfterBlendOut을 활성화 해 언제 끊기던 OnInterrupted가 브로드 캐스트 되도록합니다.
	// 클라이언트와 서버 각각 애니메이션이 종료되면 스스로 종료하도록 합니다.
	UAnimMontage* HoldMontage = SourceDataAsset->HoldMontage[SourceCharacter->GetTeam()];
	UAbilityTask_PlayMontageAndWait* PlayMontageAndWaitTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("PlayMontageAndWait"), HoldMontage);
	PlayMontageAndWaitTask->OnCancelled.AddDynamic(this, &ThisClass::K2_EndAbility);
	PlayMontageAndWaitTask->OnBlendOut.AddDynamic(this, &ThisClass::K2_EndAbility);
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
	}
}

void USMGA_Hold::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
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
	ServerRPCRequestHold(StartLocation, CursorLocation);
}

void USMGA_Hold::ServerRPCRequestHold_Implementation(const FVector_NetQuantize10& InStartLocation, const FVector_NetQuantize10& InCursorLocation)
{
	StartLocation = InStartLocation;
	CursorLocation = InCursorLocation;
	USMAT_WaitHoldResult* HoldResultTask = USMAT_WaitHoldResult::WaitHoldResult(this, StartLocation, CursorLocation, MaxDistance, LimitAngle, UnconditionalHoldDistance);
	HoldResultTask->OnFinish.BindUObject(this, &ThisClass::OnHold);
	HoldResultTask->ReadyForActivation();
}

void USMGA_Hold::OnHold(AActor* TargetActor)
{
	ASMPlayerCharacterBase* SourceCharacter = GetAvatarActor<ASMPlayerCharacterBase>();
	USMHIC_Character* SourceHIC = GetHIC<USMHIC_Character>();
	if (!SourceCharacter || !ensureAlways(SourceHIC))
	{
		EndAbilityByCancel();
		return;
	}

	bool bSuccess = false;
	USMHoldInteractionComponent* TargetHIC = USMHoldInteractionBlueprintLibrary::GetHoldInteractionComponent(TargetActor);
	if (TargetHIC)
	{
		bSuccess = true;

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

	bSuccessHold = bSuccess;
	ClientRPCSendHoldResult(bSuccessHold);
	SyncPointHoldResult();

	if (bShowDebug)
	{
		const FColor Color = bSuccessHold ? FColor::Green : FColor::Red;
		DrawDebugSphere(GetWorld(), StartLocation, MaxDistance, 16, Color, false, 2.0f);
	}
}


void USMGA_Hold::ClientRPCSendHoldResult_Implementation(bool bNewSuccessHold)
{
	bSuccessHold = bNewSuccessHold;
	SyncPointHoldResult();
}

void USMGA_Hold::SyncPointHoldResult()
{
	UAbilityTask_NetworkSyncPoint* SyncPoint = UAbilityTask_NetworkSyncPoint::WaitNetSync(this, EAbilityTaskNetSyncType::BothWait);
	SyncPoint->OnSync.AddDynamic(this, &ThisClass::PlayResultMontage);
	SyncPoint->ReadyForActivation();
}

void USMGA_Hold::PlayResultMontage()
{
	const FName SectionName = bSuccessHold ? TEXT("Success") : TEXT("Fail");
	MontageJumpToSection(SectionName);
}
