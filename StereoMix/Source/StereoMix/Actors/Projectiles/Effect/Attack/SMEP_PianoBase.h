// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Projectiles/Effect/SMEffectProjectileBase.h"
#include "SMEP_PianoBase.generated.h"

UCLASS(Abstract)
class STEREOMIX_API ASMEP_PianoBase : public ASMEffectProjectileBase
{
	GENERATED_BODY()

protected:
	virtual void HandleHitEffect(AActor* InTarget) override;
};
