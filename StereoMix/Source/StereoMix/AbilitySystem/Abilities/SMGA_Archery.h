// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMGA_Attack.h"
#include "SMGA_Archery.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class STEREOMIX_API USMGA_Archery : public USMGA_Attack
{
	GENERATED_BODY()

public:
	USMGA_Archery();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

	UFUNCTION()
	void OnCharged1();

	UFUNCTION()
	void OnCharged2();

	UPROPERTY(EditAnywhere, Category = "Design")
	float Charge1Time = 0.25f;

	UPROPERTY(EditAnywhere, Category = "Design")
	float Charge2Time = 0.5f;

	int32 ChargeLevel = 0;
};
