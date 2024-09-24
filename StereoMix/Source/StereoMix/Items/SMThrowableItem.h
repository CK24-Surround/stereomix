// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SMThrowableItem.generated.h"

class UNiagaraComponent;
class ASMHoldableItemBase;

UCLASS(Abstract)
class STEREOMIX_API ASMThrowableItem : public AActor
{
	GENERATED_BODY()

public:
	ASMThrowableItem();

	void SetThrowItem(const FVector& InLaunchVelocity, const FVector& InInitialLocation, const FVector& InTargetLocation, const float& InGravity);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Root")
	TObjectPtr<USceneComponent> SceneComponent;

	UPROPERTY(VisibleAnywhere, Category = "Niagara")
	TObjectPtr<UNiagaraComponent> NiagaraComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	TSubclassOf<ASMHoldableItemBase> ItemToSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float DestroyTime;
 
	uint8 bEnableThrow:1;
	
	float ThrowStartTime;

	FVector LaunchVelocity;

	FVector InitialLocation;

	FVector TargetLocation;
	
	float ThrowGravity;
	
public:
	virtual void Tick(float DeltaTime) override;
};
