// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMGameplayAbility.h"

#include "SMGA_Immune.generated.h"

class UFMODEvent;
/**
 *
 */
UCLASS(Abstract)
class STEREOMIX_API USMGA_Immune : public USMGameplayAbility
{
	GENERATED_BODY()

public:
	USMGA_Immune();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION()
	void OnFinishDelay();

	UPROPERTY(EditAnywhere, Category = "Design|FX|Immune")
	TObjectPtr<UFMODEvent> ImmuneSound;

	UPROPERTY(EditAnywhere, Category = "Design|Immune")
	float MoveSpeedMultiply = 1.5f;

	UPROPERTY(EditAnywhere, Category = "Design|Immune")
	float ImmuneTime = 3.0f;
};
