// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMEP_PianoBase.h"
#include "SMEP_PianoCharge1.generated.h"

UCLASS(Abstract)
class STEREOMIX_API ASMEP_PianoCharge1 : public ASMEP_PianoBase
{
	GENERATED_BODY()

protected:
	virtual void AddProjectileFX() override;

	virtual void RemoveProjectileFX() override;

	virtual void PlayHitFX(AActor* InTarget) override;

	virtual void PlayWallHitFX(const FVector& HitLocation) override;
};
