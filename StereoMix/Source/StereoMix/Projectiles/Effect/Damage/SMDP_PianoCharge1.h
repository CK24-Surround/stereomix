// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMDP_PianoBase.h"
#include "SMDP_PianoCharge1.generated.h"

UCLASS()
class STEREOMIX_API ASMDP_PianoCharge1 : public ASMDP_PianoBase
{
	GENERATED_BODY()

protected:
	virtual void AddProjectileFX() override;

	virtual void RemoveProjectileFX() override;

	virtual void PlayHitFX(AActor* InTarget) override;

	virtual void PlayWallHitFX(const FVector& HitLocation) override;
};
