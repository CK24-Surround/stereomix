// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Static.h"
#include "Data/SMTeam.h"
#include "SMGameplayCueNotifyBase.generated.h"

class UFMODEvent;
class UNiagaraSystem;

/**
 *
 */
UCLASS(Abstract)
class STEREOMIX_API USMGameplayCueNotifyBase : public UGameplayCueNotify_Static
{
	GENERATED_BODY()

public:
	USMGameplayCueNotifyBase();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FX")
	TMap<ESMTeam, TObjectPtr<UNiagaraSystem>> VFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FX")
	TMap<ESMTeam, TObjectPtr<UFMODEvent>> SFX;
};
