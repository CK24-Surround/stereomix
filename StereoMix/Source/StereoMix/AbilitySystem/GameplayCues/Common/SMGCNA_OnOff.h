// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FMODBlueprintStatics.h"
#include "AbilitySystem/GameplayCues/SMGameplayCueNotifyActorBase.h"
#include "SMGCNA_OnOff.generated.h"

class UNiagaraSystem;
class UFMODEvent;
class UNiagaraComponent;
class UFMODAudioComponent;

UCLASS()
class STEREOMIX_API ASMGCNA_OnOff : public ASMGameplayCueNotifyActorBase
{
	GENERATED_BODY()

protected:
	virtual bool OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) override;

	virtual bool OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FX")
	TMap<ESMTeam, TObjectPtr<UNiagaraSystem>> OffVFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FX")
	TMap<ESMTeam, TObjectPtr<UFMODEvent>> OffSFX;

	UPROPERTY()
	TObjectPtr<UNiagaraComponent> VFXComponent;

	UPROPERTY()
	TObjectPtr<UFMODAudioComponent> SFXComponent;

	UPROPERTY(EditAnywhere, Category = "Design")
	uint32 bUseRotationAttach:1 = false;

	FFMODEventInstance SFXInstance;
};
