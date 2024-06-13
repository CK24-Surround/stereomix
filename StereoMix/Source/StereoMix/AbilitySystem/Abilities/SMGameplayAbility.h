// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "SMGameplayAbility.generated.h"

class USMAbilitySystemComponent;
class ASMPlayerCharacter;

/**
 * 
 */
UCLASS()
class STEREOMIX_API USMGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	USMGameplayAbility();

	USMAbilitySystemComponent* GetSMAbilitySystemComponentFromActorInfo() const;

	ASMPlayerCharacter* GetSMPlayerCharacterFromActorInfo() const;

	/** 리플리케이션을 활성화 한 경우에만 호출됩니다.*/
	UFUNCTION(Client, Reliable)
	void ClientRPCPlayMontage(UAnimMontage* InMontage, float InPlayRate = 1.0f, FName StartSection = NAME_None);

protected:
	UFUNCTION()
	void EndAbilityByCancel();
};
