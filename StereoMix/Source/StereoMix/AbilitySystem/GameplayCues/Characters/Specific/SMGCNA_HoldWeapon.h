// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/GameplayCues/SMGameplayCueNotifyActorBase.h"
#include "Data/SMTeam.h"
#include "SMGCNA_HoldWeapon.generated.h"

class UNiagaraComponent;

UCLASS()
class STEREOMIX_API ASMGCNA_HoldWeapon : public ASMGameplayCueNotifyActorBase
{
	GENERATED_BODY()

public:
	ASMGCNA_HoldWeapon();

protected:
	virtual bool OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) override;

	virtual bool OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) override;

	UPROPERTY(EditAnywhere, Category = "Design")
	TMap<ESMTeam, TObjectPtr<UMaterialInterface>> OverlayMaterial;

	UPROPERTY()
	TObjectPtr<UNiagaraComponent> VFXComponent;
};
