// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMGameplayCueNotifyLoopWithTeam.h"
#include "SMGameplayCueNotifyLoopWithTeam_Stun.generated.h"

UCLASS()
class STEREOMIX_API ASMGameplayCueNotifyLoopWithTeam_Stun : public ASMGameplayCueNotifyLoopWithTeam
{
	GENERATED_BODY()

protected:
	virtual void OnStartFX(AActor* SourceActor, const FGameplayCueParameters& Parameters) const override;

	virtual void OnLoopingStartFX(AActor* SourceActor, const FGameplayCueParameters& Parameters) override;

	virtual void OnEndFX(AActor* SourceActor, const FGameplayCueParameters& Parameters) const override;
};
