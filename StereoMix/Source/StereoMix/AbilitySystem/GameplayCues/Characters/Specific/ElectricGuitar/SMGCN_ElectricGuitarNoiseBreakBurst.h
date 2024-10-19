// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/GameplayCues/SMGameplayCueNotifyBase.h"
#include "SMGCN_ElectricGuitarNoiseBreakBurst.generated.h"

/**
 * 
 */
UCLASS()
class STEREOMIX_API USMGCN_ElectricGuitarNoiseBreakBurst : public USMGameplayCueNotifyBase
{
	GENERATED_BODY()

protected:
	virtual bool OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const override;

	UPROPERTY(EditAnywhere, Category = "VFX")
	TObjectPtr<UNiagaraSystem> HealPackVFX;

	UPROPERTY(EditAnywhere, Category = "Design")
	float DelayStep = 0.016f;
};
