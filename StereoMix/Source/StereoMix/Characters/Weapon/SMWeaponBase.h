// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SMWeaponBase.generated.h"

UCLASS(Abstract)
class STEREOMIX_API ASMWeaponBase : public AActor
{
	GENERATED_BODY()

public:
	ASMWeaponBase();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Component")
	TObjectPtr<USceneComponent> SceneComponent;
};
