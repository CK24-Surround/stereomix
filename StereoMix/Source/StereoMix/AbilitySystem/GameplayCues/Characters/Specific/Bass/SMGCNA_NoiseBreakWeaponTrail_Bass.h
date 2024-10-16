// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/GameplayCues/SMGameplayCueNotifyActorBase.h"
#include "SMGCNA_NoiseBreakWeaponTrail_Bass.generated.h"

class UFMODAudioComponent;
class UNiagaraComponent;

UCLASS()
class STEREOMIX_API ASMGCNA_NoiseBreakWeaponTrail_Bass : public ASMGameplayCueNotifyActorBase
{
	GENERATED_BODY()

protected:
	virtual bool OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) override;

	virtual bool OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX")
	TObjectPtr<UNiagaraSystem> HealPackVFX;

	UPROPERTY()
	TObjectPtr<UNiagaraComponent> VFXComponent;

	UPROPERTY()
	TObjectPtr<UFMODAudioComponent> SFXComponent;
};
