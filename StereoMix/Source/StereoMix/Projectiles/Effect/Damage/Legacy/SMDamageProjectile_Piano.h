// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Projectiles/Effect/Damage/SMDamageProjectileBase.h"

#include "SMDamageProjectile_Piano.generated.h"

UCLASS()
class STEREOMIX_API ASMDamageProjectile_Piano : public ASMDamageProjectileBase
{
	GENERATED_BODY()

public:
	ASMDamageProjectile_Piano();

protected:
	virtual void MulticastStartLifeTimeInternal_Implementation() override;

	virtual void MulticastEndLifeTimeInternal_Implementation() override;

	virtual void PlayHitFX(AActor* InTarget) override;
};
