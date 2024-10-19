// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SMTutorialWall.generated.h"

class UBoxComponent;

UCLASS(Abstract)
class STEREOMIX_API ASMTutorialWall : public AActor
{
	GENERATED_BODY()

public:
	ASMTutorialWall();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Root")
	TObjectPtr<USceneComponent> RootSceneComponent;

	UPROPERTY(VisibleAnywhere, Category = "Visual")
	TObjectPtr<UStaticMeshComponent> WallComponent;
};
