// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMGameplayCueNotify_Actor.h"
#include "Utilities/SMTeam.h"
#include "SMGameplayCueNotify_Actor_WithTeam.generated.h"

class UNiagaraSystem;

UCLASS()
class STEREOMIX_API ASMGameplayCueNotify_Actor_WithTeam : public ASMGameplayCueNotify_Actor
{
	GENERATED_BODY()

public:
	ASMGameplayCueNotify_Actor_WithTeam();

protected:
	/** 루트를 기준으로 어태치하고 재생합니다. */
	virtual void PlayOnActivateNiagaraSystemWithAttach(AActor* SourceActor, const FGameplayCueParameters& Parameters) const override;

	/** 파라미터의 어태치 타겟을 기준으로 어태치하고 재생합니다. */
	virtual void PlayOnLoopingStartNiagaraSystemWithAttach(AActor* SourceActor, const FGameplayCueParameters& Parameters) override;

	/** 루트를 기준으로 어태치하고 재생합니다. */
	virtual void PlayOnRemoveNiagaraSystemWithAttach(AActor* SourceActor, const FGameplayCueParameters& Parameters) const override;

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

	/** 가해자 팀의 이펙트 사용 여부입니다. false인 경우 자신의 팀 이펙트를 사용합니다. */
	UPROPERTY(EditAnywhere, Category = "FX")
	uint32 bUseInstigatorTeam:1 = false;
};
