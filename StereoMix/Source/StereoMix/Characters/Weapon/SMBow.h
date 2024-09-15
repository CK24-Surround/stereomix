// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMWeaponBase.h"
#include "SMBow.generated.h"

UCLASS(Abstract)
class STEREOMIX_API ASMBow : public ASMWeaponBase
{
	GENERATED_BODY()

public:
	ASMBow();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	TObjectPtr<USkeletalMeshComponent> WeaponMesh;
};
