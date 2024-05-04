// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMGameplayCueNotifyLoop.h"
#include "Utilities/SMTeam.h"
#include "SMGameplayCueNotifyLoopWithTeam.generated.h"

class UNiagaraSystem;

UCLASS(Abstract)
class STEREOMIX_API ASMGameplayCueNotifyLoopWithTeam : public ASMGameplayCueNotifyLoop
{
	GENERATED_BODY()

public:
	ASMGameplayCueNotifyLoopWithTeam();

protected:
	void GetLocationAndRotation(const FGameplayCueParameters& Parameters, FVector& OutLocation, FRotator& OutRotation) const;

	ESMTeam GetTeamForSource(const AActor* SourceActor) const;

protected:
	UPROPERTY(EditAnywhere, Category = "FX")
	TMap<ESMTeam, TObjectPtr<UNiagaraSystem>> StartFX;

	UPROPERTY(EditAnywhere, Category = "FX")
	TMap<ESMTeam, TObjectPtr<UNiagaraSystem>> LoopFX;

	UPROPERTY(EditAnywhere, Category = "FX")
	TMap<ESMTeam, TObjectPtr<UNiagaraSystem>> EndFX;
};
