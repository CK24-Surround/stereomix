// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "SMJumpPad.generated.h"

class UBoxComponent;

UCLASS()
class STEREOMIX_API ASMJumpPad : public AActor
{
	GENERATED_BODY()

public:
	ASMJumpPad();

protected:
	virtual void PostInitializeComponents() override;

protected:
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

protected:
	void Jump(ACharacter* InCharacter, const FVector& TargetLocation);

protected:
	UPROPERTY(VisibleAnywhere, Category = "Root")
	TObjectPtr<USceneComponent> SceneComponent;

	UPROPERTY(VisibleAnywhere, Category = "Collision")
	TObjectPtr<UBoxComponent> BoxComponent;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(VisibleAnywhere, Category = "JumpTarget")
	TObjectPtr<USceneComponent> JumpTarget;

protected:
	UPROPERTY(EditAnywhere, Category = "Jump")
	float GravityScale = 4.0f;

	UPROPERTY(EditAnywhere, Category = "Jump")
	float ApexHeight = 500.0f;

	UPROPERTY(EditAnywhere, Category = "GAS|Tags")
	FGameplayTagContainer DenineTags;
};
