// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/GameplayCues/SMGameplayCueNotifyActorBase.h"
#include "SMGCNA_Immune.generated.h"

class UNiagaraComponent;

UCLASS()
class STEREOMIX_API ASMGCNA_Immune : public ASMGameplayCueNotifyActorBase
{
	GENERATED_BODY()

public:
	ASMGCNA_Immune();

protected:
	virtual bool OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) override;

	virtual bool OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) override;
	
	UPROPERTY(EditAnywhere, Category = "Design")
	TMap<ESMTeam, TObjectPtr<UMaterialInterface>> ImmuneMaterial;

	UPROPERTY(EditAnywhere, Category = "Design")
	TMap<ESMTeam, TObjectPtr<UMaterialInterface>> ImmuneOverlayMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FX")
	TMap<ESMTeam, TObjectPtr<UNiagaraSystem>> EndVFX;

	UPROPERTY()
	TObjectPtr<UNiagaraComponent> VFXComponent;
};
