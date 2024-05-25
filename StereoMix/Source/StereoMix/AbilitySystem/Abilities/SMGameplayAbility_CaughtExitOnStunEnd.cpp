// Copyright Surround, Inc. All Rights Reserved.


#include "SMGameplayAbility_CaughtExitOnStunEnd.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Characters/SMPlayerCharacter.h"
#include "AbilitySystem/SMTags.h"

USMGameplayAbility_CaughtExitOnStunEnd::USMGameplayAbility_CaughtExitOnStunEnd()
{
	AbilityTags = FGameplayTagContainer(SMTags::Ability::CaughtExitOnStunEnd);
}

void USMGameplayAbility_CaughtExitOnStunEnd::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (!bWasCancelled)
	{
		ASMPlayerCharacter* SourceCharacter = GetSMPlayerCharacterFromActorInfo();
		if (SourceCharacter)
		{
			// 스턴 어빌리티에게 잡기 탈출이 끝났음을 알립니다.
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(SourceCharacter, SMTags::Event::Character::CaughtExitEnd, FGameplayEventData());
		}
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
