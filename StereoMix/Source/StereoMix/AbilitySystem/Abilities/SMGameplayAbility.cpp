// Copyright Surround, Inc. All Rights Reserved.


#include "SMGameplayAbility.h"

#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "Characters/SMPlayerCharacter.h"
#include "Characters/Player/SMPlayerCharacterBase.h"
#include "Utilities/SMLog.h"

USMGameplayAbility::USMGameplayAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateNo;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ClientOrServer;
}

USMAbilitySystemComponent* USMGameplayAbility::GetSMAbilitySystemComponentFromActorInfo() const
{
	if (!ensureAlways(CurrentActorInfo))
	{
		return nullptr;
	}

	USMAbilitySystemComponent* SourceASC = Cast<USMAbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent.Get());
	if (!SourceASC)
	{
		NET_LOG(nullptr, Error, TEXT("소스 ASC가 유효하지 않습니다."));
		return nullptr;
	}

	return SourceASC;
}

ASMPlayerCharacter* USMGameplayAbility::GetSMPlayerCharacterFromActorInfo() const
{
	if (!ensureAlways(CurrentActorInfo))
	{
		return nullptr;
	}

	ASMPlayerCharacter* SourceCharacter = Cast<ASMPlayerCharacter>(CurrentActorInfo->AvatarActor.Get());
	if (!SourceCharacter)
	{
		NET_LOG(nullptr, Error, TEXT("소스 캐릭터가 유효하지 않습니다."));
		return nullptr;
	}

	return SourceCharacter;
}

UAbilitySystemComponent* USMGameplayAbility::GetASC() const
{
	if (!ensureAlways(CurrentActorInfo))
	{
		return nullptr;
	}

	return CurrentActorInfo->AbilitySystemComponent.Get();
}

AActor* USMGameplayAbility::GetAvatarActor() const
{
	if (!ensureAlways(CurrentActorInfo))
	{
		return nullptr;
	}

	return CurrentActorInfo->AvatarActor.Get();
}

USMHoldInteractionComponent* USMGameplayAbility::GetHIC() const
{
	if (!ensureAlways(CurrentActorInfo))
	{
		return nullptr;
	}

	return USMHoldInteractionBlueprintLibrary::GetHoldInteractionComponent(CurrentActorInfo->AvatarActor.Get());
}

const USMPlayerCharacterDataAsset* USMGameplayAbility::GetDataAsset() const
{
	if (!ensureAlways(CurrentActorInfo))
	{
		return nullptr;
	}

	ASMPlayerCharacterBase* SourceCharacter = Cast<ASMPlayerCharacterBase>(CurrentActorInfo->AvatarActor.Get());
	if (!SourceCharacter)
	{
		return nullptr;
	}

	return SourceCharacter->GetDataAsset();
}

void USMGameplayAbility::EndAbilityByCancel()
{
	check(CurrentActorInfo);

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void USMGameplayAbility::ClientRPCPlayMontage_Implementation(UAnimMontage* InMontage, float InPlayRate, FName StartSection)
{
	if (ensure(ReplicationPolicy == EGameplayAbilityReplicationPolicy::ReplicateYes))
	{
		USMAbilitySystemComponent* SourceASC = GetSMAbilitySystemComponentFromActorInfo();
		if (ensure(SourceASC))
		{
			const float Duration = SourceASC->PlayMontage(this, CurrentActivationInfo, InMontage, InPlayRate, StartSection);
			ensure(Duration > 0.0f);
		}
	}
}
