// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMDamageProjectile.h"

#include "SMDamageProjectile_Piano.generated.h"

UCLASS()
class STEREOMIX_API ASMDamageProjectile_Piano : public ASMDamageProjectile
{
	GENERATED_BODY()

public:
	ASMDamageProjectile_Piano();

protected:
	virtual void MulticastStartLifeTimeInternal_Implementation() override;

	virtual void MulticastEndLifeTimeInternal_Implementation() override;
};
