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

	virtual void PlayNiagaraSystemWithAttach(AActor* SourceActor, const FGameplayCueParameters& Parameters) const override;

protected:
	void GetLocationAndRotation(const FGameplayCueParameters& Parameters, FVector& OutLocation, FRotator& OutRotation) const;

	ESMTeam GetTeamForSource(const AActor* SourceActor) const;

protected:
	UPROPERTY(EditAnywhere, Category = "FX")
	TMap<ESMTeam, TObjectPtr<UNiagaraSystem>> FX;

	/** 가해자 팀의 이펙트 사용 여부입니다. false인 경우 자신의 팀 이펙트를 사용합니다. */
	UPROPERTY(EditAnywhere, Category = "FX")
	uint32 bUseInstigatorTeam:1 = false;
};
