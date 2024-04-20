// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "SMHealPack.generated.h"

class USphereComponent;
class UGameplayEffect;

UCLASS(Abstract)
class STEREOMIX_API ASMHealPack : public AActor
{
	GENERATED_BODY()

public:
	ASMHealPack();

protected:
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Root")
	TObjectPtr<USceneComponent> SceneComponent;

	UPROPERTY(VisibleAnywhere, Category = "Collision")
	TObjectPtr<USphereComponent> SphereComponent;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

protected:
	UPROPERTY(EditAnywhere, Category = "GE")
	TSubclassOf<UGameplayEffect> HealGE;
};
