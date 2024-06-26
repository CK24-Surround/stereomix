// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMDamageProjectile.h"
#include "SMDamageProjectile_Bass.generated.h"

UCLASS()
class STEREOMIX_API ASMDamageProjectile_Bass : public ASMDamageProjectile
{
	GENERATED_BODY()

public:
	ASMDamageProjectile_Bass();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
};
