// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Actor.h"
#include "Data/SMTeam.h"
#include "SMGameplayCueNotifyActorBase.generated.h"

class UFMODEvent;
class UNiagaraSystem;

/**
 *
 */
UCLASS(Abstract)
class STEREOMIX_API ASMGameplayCueNotifyActorBase : public AGameplayCueNotify_Actor
{
	GENERATED_BODY()

public:
	ASMGameplayCueNotifyActorBase();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FX")
	TMap<ESMTeam, TObjectPtr<UNiagaraSystem>> VFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FX")
	TMap<ESMTeam, TObjectPtr<UFMODEvent>> SFX;
};
