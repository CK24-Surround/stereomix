// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Projectiles/Effect/SMEffectProjectileBase.h"
#include "SMEP_ElectricGuitar.generated.h"

UCLASS()
class STEREOMIX_API ASMEP_ElectricGuitar : public ASMEffectProjectileBase
{
	GENERATED_BODY()

protected:
	virtual void HandleHitEffect(AActor* InTarget) override;

	virtual void AddProjectileFX() override;

	virtual void RemoveProjectileFX() override;

	virtual void PlayHitFX(AActor* InTarget) override;

	virtual void PlayWallHitFX(const FVector& HitLocation) override;
};
