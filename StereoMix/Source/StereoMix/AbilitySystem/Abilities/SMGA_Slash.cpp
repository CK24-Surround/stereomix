// Copyright Surround, Inc. All Rights Reserved.


#include "SMGA_Slash.h"

#include "Characters/SMPlayerCharacterBase.h"

void USMGA_Slash::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ASMPlayerCharacterBase* SourceCharacter = Cast<ASMPlayerCharacterBase>(GetAvatarActorFromActorInfo());
	if (!ensureAlways(SourceCharacter))
	{
		K2_EndAbility();
		return;
	}

	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.0f, FColor::Yellow, TEXT("베기!"));
	K2_EndAbility();
}
