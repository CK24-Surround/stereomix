// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "SMJumpPad.generated.h"

class UBoxComponent;

UCLASS()
class STEREOMIX_API ASMJumpPad : public AActor
{
	GENERATED_BODY()

public:
	ASMJumpPad();

protected:
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

protected:
	void Jump(ACharacter* InCharacter, const FVector& TargetLocation);

protected:
	UPROPERTY(VisibleAnywhere, Category = "Root")
	TObjectPtr<USceneComponent> SceneComponent;

	UPROPERTY(VisibleAnywhere, Category = "Collision")
	TObjectPtr<UBoxComponent> BoxComponent;
};
