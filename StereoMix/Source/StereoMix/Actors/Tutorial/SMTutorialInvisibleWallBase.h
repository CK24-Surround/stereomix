// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SMTutorialInvisibleWallBase.generated.h"

class UBoxComponent;

UCLASS(Abstract)
class STEREOMIX_API ASMTutorialInvisibleWallBase : public AActor
{
	GENERATED_BODY()

public:
	ASMTutorialInvisibleWallBase();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Root")
	TObjectPtr<USceneComponent> RootSceneComponent;

	UPROPERTY(VisibleAnywhere, Category = "Collider")
	TObjectPtr<UBoxComponent> BoxComponent; 
};
