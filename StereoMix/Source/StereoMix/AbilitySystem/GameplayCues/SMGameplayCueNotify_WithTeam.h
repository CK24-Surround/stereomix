// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/SMTeam.h"
#include "SMGameplayCueNotify.h"

#include "SMGameplayCueNotify_WithTeam.generated.h"

class UFMODEvent;
class UNiagaraSystem;
/**
 *
 */
UCLASS()
class STEREOMIX_API USMGameplayCueNotify_WithTeam : public USMGameplayCueNotify
{
	GENERATED_BODY()

protected:
	virtual void PlayNiagaraSystem(AActor* SourceActor, const FGameplayCueParameters& Parameters) const override;

	virtual void PlayNiagaraSystemWithAttach(AActor* SourceActor, const FGameplayCueParameters& Parameters) const override;

	virtual void PlaySound(AActor* SourceActor, const FGameplayCueParameters& Parameters) const override;

	virtual void PlaySoundWithAttach(AActor* SourceActor, const FGameplayCueParameters& Parameters) const override;

	ESMTeam GetTeamForSource(const AActor* SourceActor) const;

	UPROPERTY(EditAnywhere, Category = "Design|FX|Effect")
	TMap<ESMTeam, TObjectPtr<UNiagaraSystem>> Effect;

	UPROPERTY(EditAnywhere, Category = "Design|FX|Sound")
	TMap<ESMTeam, TObjectPtr<UFMODEvent>> Sound;

	/** 가해자 팀의 이펙트 사용 여부입니다. false인 경우 자신의 팀 이펙트를 사용합니다. */
	UPROPERTY(EditAnywhere, Category = "Design|FX")
	uint32 bUseInstigatorTeam:1 = false;
};
