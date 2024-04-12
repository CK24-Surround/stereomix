// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SMProjectile.generated.h"

class UGameplayEffect;
class USMDesignData;
class USphereComponent;
class UProjectileMovementComponent;

UCLASS()
class STEREOMIX_API ASMProjectile : public AActor
{
	GENERATED_BODY()

public:
	ASMProjectile();

public:
	virtual void PostInitializeComponents() override;
	
protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	
	virtual void NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

protected:
	UPROPERTY()
	TObjectPtr<USMDesignData> DesignData;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Collision")
	TObjectPtr<USphereComponent> SphereComponent;
	
	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	TObjectPtr<UStaticMeshComponent> MeshComponent;
	
	UPROPERTY(VisibleAnywhere, Category = "Projectile")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent;

protected:
	FVector StartLocation;

protected:
	UPROPERTY(EditAnywhere, Category = "GAS|GE")
	TSubclassOf<UGameplayEffect> HitGE;
};
