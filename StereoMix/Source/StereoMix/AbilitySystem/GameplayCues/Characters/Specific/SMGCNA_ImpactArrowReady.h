// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/GameplayCues/SMGameplayCueNotifyActorBase.h"
#include "SMGCNA_ImpactArrowReady.generated.h"

class UFMODAudioComponent;
class UNiagaraComponent;

UCLASS()
class STEREOMIX_API ASMGCNA_ImpactArrowReady : public ASMGameplayCueNotifyActorBase
{
	GENERATED_BODY()

public:
	ASMGCNA_ImpactArrowReady();

protected:
	virtual bool OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) override;

	virtual bool OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) override;

	UPROPERTY()
	TObjectPtr<UNiagaraComponent> VFXComponent;

	UPROPERTY()
	TObjectPtr<UFMODAudioComponent> SFXComponent;
};
