// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Actor.h"
#include "SMGameplayCueNotify_Actor.generated.h"

class UNiagaraComponent;

UCLASS(Abstract)
class STEREOMIX_API ASMGameplayCueNotify_Actor : public AGameplayCueNotify_Actor
{
	GENERATED_BODY()

protected:
	virtual bool OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) override;

	virtual bool WhileActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) override;

	virtual bool OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) override;

protected:
	virtual void PlayNiagaraSystemWithAttachOnActivate(AActor* SourceActor, const FGameplayCueParameters& Parameters) const PURE_VIRTUAL(USMGameplayCueNotify::PlayNiagaraSystemWithAttach);

	virtual void PlayNiagaraSystemWithAttachOnLoopingStart(AActor* SourceActor, const FGameplayCueParameters& Parameters) const PURE_VIRTUAL(USMGameplayCueNotify::PlayNiagaraSystemWithAttach);

	virtual void PlayNiagaraSystemOnRemove(AActor* SourceActor, const FGameplayCueParameters& Parameters) const PURE_VIRTUAL(USMGameplayCueNotify::PlayNiagaraSystem);
};
