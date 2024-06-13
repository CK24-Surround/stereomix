// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Actor.h"
#include "SMGameplayCueNotifyLoop.generated.h"

class UNiagaraComponent;

UCLASS(Abstract)
class STEREOMIX_API ASMGameplayCueNotifyLoop : public AGameplayCueNotify_Actor
{
	GENERATED_BODY()

protected:
	virtual bool OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) override;

	virtual bool WhileActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) override;

	virtual bool OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) override;

	/** FX가 시작할때 호출됩니다. 서브 클래스에서 직접 동작 방식을 구현해줘야합니다. */
	virtual void OnStartFX(AActor* SourceActor, const FGameplayCueParameters& Parameters) const PURE_VIRTUAL(USMGameplayCueNotify::PlayNiagaraSystemWithAttach);

	/** FX의 루프가 시작될때 호출됩니다. 서브 클래스에서 직접 동작 방식을 구현해줘야합니다. */
	virtual void OnLoopingStartFX(AActor* SourceActor, const FGameplayCueParameters& Parameters) PURE_VIRTUAL(USMGameplayCueNotify::PlayNiagaraSystemWithAttach);

	/** FX가 끝날때 호출됩니다. 서브 클래스에서 직접 동작 방식을 구현해줘야합니다. */
	virtual void OnEndFX(AActor* SourceActor, const FGameplayCueParameters& Parameters) const PURE_VIRTUAL(USMGameplayCueNotify::PlayNiagaraSystem);

	UPROPERTY()
	TObjectPtr<UNiagaraComponent> LoopingFXComponent;
};
