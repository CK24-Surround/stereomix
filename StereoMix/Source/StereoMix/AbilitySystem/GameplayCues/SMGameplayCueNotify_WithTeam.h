// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMGameplayCueNotify.h"
#include "Utilities/SMTeam.h"
#include "SMGameplayCueNotify_WithTeam.generated.h"

class UNiagaraSystem;
/**
 * 
 */
UCLASS()
class STEREOMIX_API USMGameplayCueNotify_WithTeam : public USMGameplayCueNotify
{
	GENERATED_BODY()

public:
	USMGameplayCueNotify_WithTeam();

protected:
	virtual void PlayNiagaraSystem(AActor* SourceActor, const FGameplayCueParameters& Parameters) const override;

protected:
	UPROPERTY(EditAnywhere, Category = "FX")
	TMap<ESMTeam, TObjectPtr<UNiagaraSystem>> FX;
};
