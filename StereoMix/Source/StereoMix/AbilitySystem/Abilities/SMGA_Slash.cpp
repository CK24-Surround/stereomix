// Copyright Surround, Inc. All Rights Reserved.


#include "SMGA_Slash.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "Characters/SMBassCharacter.h"
#include "Data/Character/SMPlayerCharacterDataAsset.h"
#include "Utilities/SMLog.h"

USMGA_Slash::USMGA_Slash()
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;
}

void USMGA_Slash::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	K2_CommitAbility();

	// Slash();
	// NET_VLOG(GetAvatarActorFromActorInfo(), -1, 3.0f, TEXT("슬래시!"));
}

void USMGA_Slash::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);

	ASMBassCharacter* SourceCharacter = Cast<ASMBassCharacter>(GetAvatarActorFromActorInfo());
	if (!ensureAlways(SourceCharacter))
	{
		return;
	}

	// if (SourceCharacter->GetCanInput())
	// {
	// 	NET_VLOG(GetAvatarActorFromActorInfo(), -1, 3.0f, TEXT("입력!"));
	// 	SourceCharacter->SetIsInput(true);
	// }
}

void USMGA_Slash::Slash()
{
	ASMBassCharacter* SourceCharacter = Cast<ASMBassCharacter>(GetAvatarActorFromActorInfo());
	if (!ensureAlways(SourceCharacter))
	{
		EndAbilityByCancel();
		return;
	}

	USMAbilitySystemComponent* SourceASC = GetSMAbilitySystemComponentFromActorInfo();
	if (!ensureAlways(SourceASC))
	{
		EndAbilityByCancel();
		return;
	}

	// ESMTeam SourceTeam = SourceCharacter->GetTeam();
	// const USMPlayerCharacterDataAsset* CharacterDataAsset = SourceCharacter->GetDataAsset();
	// if (CharacterDataAsset)
	// {
	// 	NET_LOG(SourceCharacter, Warning, TEXT("베기 애니메이션 시작"));
	//
	// 	const bool bIsLeftSlash = SourceCharacter->GetIsLeftSlash();
	// 	const FName SlashDirectionName = bIsLeftSlash ? TEXT("Left") : TEXT("Right");
	// 	UAnimMontage* SlashMontage = CharacterDataAsset->AttackMontage[SourceTeam];
	// 	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("Slash"), SlashMontage, 1.0f, SlashDirectionName);
	//
	// 	MontageTask->OnInterrupted.AddDynamic(this, &ThisClass::InterruptedTest);
	// 	MontageTask->OnCompleted.AddDynamic(this, &ThisClass::CompletedTest);
	//
	// 	if (K2_HasAuthority())
	// 	{
	// 		MontageTask->OnInterrupted.AddDynamic(this, &ThisClass::SlashAnimationEnded);
	// 		MontageTask->OnCompleted.AddDynamic(this, &ThisClass::SlashAnimationEnded);
	// 		SourceCharacter->SetIsLeftSlash(!bIsLeftSlash);
	// 	}
	//
	// 	MontageTask->ReadyForActivation();
	// }

	// if (K2_HasAuthority())
	// {
	// 	SourceCharacter->Slash();
	// }
	// else
	// {
	// 	SourceCharacter->OnSlash.BindUObject(this, &ThisClass::SlashRequest);
	// }
}

void USMGA_Slash::SlashEnded()
{
	ASMBassCharacter* SourceCharacter = Cast<ASMBassCharacter>(GetAvatarActorFromActorInfo());
	if (!ensureAlways(SourceCharacter))
	{
		K2_EndAbility();
		return;
	}
}

void USMGA_Slash::SlashRequest()
{
	ServerRPCSlashRequest();
	Slash();
}

void USMGA_Slash::ServerRPCSlashRequest_Implementation()
{
	Slash();
}

void USMGA_Slash::SlashAnimationEnded()
{
	// ASMBassCharacter* SourceCharacter = Cast<ASMBassCharacter>(GetAvatarActorFromActorInfo());
	// if (SourceCharacter)
	// {
	// 	SourceCharacter->OnSlash.Unbind();
	// 	SourceCharacter->SetIsLeftSlash(true);
	// }

	K2_EndAbility();
}

void USMGA_Slash::CancelledTest()
{
	NET_LOG(GetAvatarActorFromActorInfo(), Warning, TEXT("CancelledTest"));
}

void USMGA_Slash::InterruptedTest()
{
	NET_LOG(GetAvatarActorFromActorInfo(), Warning, TEXT("InterruptedTest"));
}

void USMGA_Slash::CompletedTest()
{
	NET_LOG(GetAvatarActorFromActorInfo(), Warning, TEXT("CompletedTest"));
}
