// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/GameplayCues/SMGameplayCueNotifyActorBase.h"
#include "SMGCNA_Archery.generated.h"

class UFMODAudioComponent;
class UNiagaraComponent;

UCLASS()
class STEREOMIX_API ASMGCNA_Archery : public ASMGameplayCueNotifyActorBase
{
	GENERATED_BODY()

public:
	ASMGCNA_Archery();

protected:
	virtual bool OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) override;

	virtual bool OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) override;

	UPROPERTY(EditAnywhere, Category = "Design")
	FVector Offset = FVector(10.0, 0.0, 22.0);

	UPROPERTY()
	TObjectPtr<UNiagaraComponent> VFXComponent;

	UPROPERTY()
	TObjectPtr<UFMODAudioComponent> SFXComponent;
};
