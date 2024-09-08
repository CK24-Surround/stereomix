// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMGameplayAbility.h"
#include "SMGA_Slash.generated.h"

/**
 * 
 */
UCLASS()
class STEREOMIX_API USMGA_Slash : public USMGameplayAbility
{
	GENERATED_BODY()

public:
	USMGA_Slash();

	uint32 bCanInput:1 = false;
	uint32 bIsInput:1 = false;
	uint32 bCanProceedNextAction:1 = false;
	uint32 bIsLeftSlashNext:1 = true;

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

	void FocusToCursor();

	UFUNCTION()
	void SlashEnded();

	UFUNCTION()
	void OnSlashJudgeStartCallback(FGameplayEventData Payload);

	UFUNCTION()
	void CanInputCallback(FGameplayEventData Payload) { bCanInput = true; }

	UFUNCTION()
	void CanProceedNextActionCallback(FGameplayEventData Payload) { bCanProceedNextAction = true; }

	UFUNCTION()
	void LeftSlashNextCallback(FGameplayEventData Payload) { bIsLeftSlashNext = true; }

	UFUNCTION()
	void RightSlashNextCallback(FGameplayEventData Payload) { bIsLeftSlashNext = false; }

	void OnNextActionProcced();
};
