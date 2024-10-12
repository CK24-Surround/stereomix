// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Projectiles/Effect/SMEffectProjectileBase.h"
#include "SMEP_SlowBullet.generated.h"

UCLASS(Abstract)
class STEREOMIX_API ASMEP_SlowBullet : public ASMEffectProjectileBase
{
	GENERATED_BODY()

protected:
	virtual void AddProjectileFX() override;

	virtual void RemoveProjectileFX() override;

	virtual void PlayHitFX(AActor* InTarget) override;

	virtual void PlayWallHitFX(const FVector& HitLocation) override;
};
