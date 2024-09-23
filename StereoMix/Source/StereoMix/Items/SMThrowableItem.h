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

	void SetThrowItem(const FVector& InLaunchVelocity, const FVector& InInitialLocation, const FVector& InTargetLocation, const float& InDesiredTime);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Root")
	TObjectPtr<USceneComponent> SceneComponent;

	UPROPERTY(VisibleAnywhere, Category = "Niagara")
	TObjectPtr<UNiagaraComponent> NiagaraComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	TSubclassOf<ASMHoldableItemBase> Item;

	UPROPERTY(Replicated)
	uint8 bEnableThrow:1;
	
	UPROPERTY(Replicated)
	float ThrowStartTime;

	UPROPERTY(Replicated)
	FVector LaunchVelocity;

	UPROPERTY(Replicated)
	FVector InitialLocation;

	UPROPERTY(Replicated)
	FVector TargetLocation;
	
	UPROPERTY(Replicated)
	float ThrowGravity;
	
public:
	virtual void Tick(float DeltaTime) override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
