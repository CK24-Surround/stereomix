// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SMObstacleBase.generated.h"

class UNiagaraComponent;
class UBoxComponent;

UCLASS(Abstract)
class STEREOMIX_API ASMObstacleBase : public AActor
{
	GENERATED_BODY()

public:
	ASMObstacleBase();

	virtual void SetCollisionEnabled(bool bNewIsCollisionEnabled);
	
protected:
	UPROPERTY(VisibleAnywhere, Category = "Collider")
	TObjectPtr<UBoxComponent> ColliderComponent;

	UPROPERTY(VisibleAnywhere, Category = "Visual")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(VisibleAnywhere, Category = "Visual")
	TObjectPtr<UNiagaraComponent> NiagaraComponent;
};
