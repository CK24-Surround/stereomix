// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/GameplayCues/SMGameplayCueNotifyBase.h"
#include "SMGCN_NoiseBreakBurst_Baas.generated.h"

/**
 * 
 */
UCLASS()
class STEREOMIX_API USMGCN_NoiseBreakBurst_Baas : public USMGameplayCueNotifyBase
{
	GENERATED_BODY()

protected:
	virtual bool OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX")
	TObjectPtr<UNiagaraSystem> HealPackVFX;
};