// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Static.h"

#include "SMGameplayCueNotify.generated.h"

/**
 *
 */
UCLASS(Abstract)
class STEREOMIX_API USMGameplayCueNotify : public UGameplayCueNotify_Static
{
	GENERATED_BODY()

protected:
	virtual bool OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const override;

	virtual void PlayNiagaraSystem(AActor* SourceActor, const FGameplayCueParameters& Parameters) const PURE_VIRTUAL(USMGameplayCueNotify::PlayNiagaraSystem)

	virtual void PlayNiagaraSystemWithAttach(AActor* SourceActor, const FGameplayCueParameters& Parameters) const PURE_VIRTUAL(USMGameplayCueNotify::PlayNiagaraSystemWithAttach)

	virtual void PlaySound(AActor* SourceActor, const FGameplayCueParameters& Parameters) const PURE_VIRTUAL(USMGameplayCueNotify::PlaySound)

	virtual void PlaySoundWithAttach(AActor* SourceActor, const FGameplayCueParameters& Parameters) const PURE_VIRTUAL(USMGameplayCueNotify::PlaySound)

	/** 큐 파라미터에서 위치와 회전값을 가져옵니다. */
	void GetLocationAndRotation(const FGameplayCueParameters& Parameters, FVector& OutLocation, FRotator& OutRotation) const;

	/** 어태치 수행 여부를 결정합니다. */
	UPROPERTY(EditAnywhere, Category = "FX")
	uint32 bIsAttach:1 = false;
};
