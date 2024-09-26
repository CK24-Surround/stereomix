// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SMThrowableItem.generated.h"

class ASMItemBase;
class UNiagaraComponent;

UCLASS(Abstract)
class STEREOMIX_API ASMThrowableItem : public AActor
{
	GENERATED_BODY()

public:
	ASMThrowableItem();

	void SetSpawnItem(TSubclassOf<ASMItemBase> InItemToSpawn);
	
	void SetAttribute(const FVector& InLaunchVelocity, const FVector& InInitialLocation, const FVector& InTargetLocation, const float& InGravity);

protected:
	virtual void Tick(float DeltaTime) override;

	void OnTargetArrived();

	UPROPERTY(VisibleAnywhere, Category = "Root")
	TObjectPtr<USceneComponent> SceneComponent;

	UPROPERTY(VisibleAnywhere, Category = "Niagara")
	TObjectPtr<UNiagaraComponent> NiagaraComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Design")
	TSubclassOf<ASMItemBase> ItemToSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Design")
	float DestroyTime = 15.0f;

	uint8 bEnableThrow:1 = false;

	float ThrowStartTime = 0.0f;

	FVector LaunchVelocity;

	FVector InitialLocation;

	FVector TargetLocation;

	float ThrowGravity = 0.0f;
};
