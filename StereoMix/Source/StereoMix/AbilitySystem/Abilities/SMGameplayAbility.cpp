// Copyright Surround, Inc. All Rights Reserved.


#include "SMGameplayAbility.h"

#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "Characters/SMPlayerCharacter.h"
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
