// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SMDestroyableObstacle.generated.h"

UCLASS(Abstract)
class STEREOMIX_API ASMDestroyableObstacle : public AActor
{
	GENERATED_BODY()

public:
	ASMDestroyableObstacle();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(VisibleAnywhere, Category = "Root")
	TObjectPtr<USceneComponent> SceneComponent;

	UPROPERTY(VisibleAnywhere, Category = "Visual")
	TObjectPtr<UStaticMeshComponent> MeshComponent;
	
	UPROPERTY(EditAnywhere, Category = "Design")
	float CurrentHealth = 100.0f;
	
	UPROPERTY(EditAnywhere, Category = "Design")
	float MaxHealth = 100.0f;

public:
	void HandleDamage(float InDamage);
};
