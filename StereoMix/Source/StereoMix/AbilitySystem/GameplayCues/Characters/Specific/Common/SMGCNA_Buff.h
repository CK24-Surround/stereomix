// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/GameplayCues/SMGameplayCueNotifyActorBase.h"
#include "SMGCNA_Buff.generated.h"

class UNiagaraComponent;

UCLASS()
class STEREOMIX_API ASMGCNA_Buff : public ASMGameplayCueNotifyActorBase
{
	GENERATED_BODY()

public:
	ASMGCNA_Buff();

protected:
	virtual bool OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) override;

	virtual bool OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FX")
	TMap<ESMTeam, TObjectPtr<UNiagaraSystem>> LoopVFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Design")
	TMap<ESMTeam, TObjectPtr<UMaterialInterface>> OverlayMaterial;

	UPROPERTY()
	TObjectPtr<UNiagaraComponent> LoopVFXComponent;
};
