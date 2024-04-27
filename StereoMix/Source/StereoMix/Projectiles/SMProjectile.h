// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "Utilities/SMTeam.h"
#include "SMProjectile.generated.h"

DECLARE_DELEGATE_OneParam(FOnProjectileLifeTimeSignature, ASMProjectile* /*Projectile*/);

class UNiagaraComponent;
class UNiagaraSystem;
class USMProjectileAssetData;
class USMTeamComponent;
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

public:
	void Launch(AActor* NewOwner, const FVector_NetQuantize10& InStartLocation, const FVector_NetQuantize10& InNormal, float InSpeed);

protected:
	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastRPCLaunch(const FVector_NetQuantize10& InStartLocation, const FVector_NetQuantize10& InNormal, float InSpeed);

	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastRPCStartLifeTime();

	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastRPCEndLifeTime();

protected:
	UPROPERTY()
	TObjectPtr<USMDesignData> DesignData;

	UPROPERTY()
	TObjectPtr<USMProjectileAssetData> AssetData;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Collision")
	TObjectPtr<USphereComponent> SphereComponent;

	UPROPERTY(VisibleAnywhere, Category = "FX")
	TObjectPtr<UNiagaraComponent> ProjectileFXComponent;

	UPROPERTY(VisibleAnywhere, Category = "Projectile")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent;

	UPROPERTY(VisibleAnywhere, Category = "Team")
	TObjectPtr<USMTeamComponent> TeamComponent;

	UPROPERTY(EditAnywhere, Category = "FX")
	TMap<ESMTeam, TObjectPtr<UNiagaraSystem>> ProjectileFX;

	UPROPERTY(EditAnywhere, Category = "FX")
	TMap<ESMTeam, TObjectPtr<UNiagaraSystem>> ProjectileHitFX;

public:
	/** 생명 주기가 끝나 회수되야할때 호출됩니다. Destroy를 대체합니다. */
	FOnProjectileLifeTimeSignature OnEndLifeTime;

protected:
	FVector StartLocation;

protected:
	UPROPERTY(EditAnywhere, Category = "GAS|Tags")
	FGameplayTagContainer IgnoreTargetStateTags;

	UPROPERTY(EditAnywhere, Category = "GAS|GE")
	TSubclassOf<UGameplayEffect> HitGE;

public:
	UFUNCTION()
	void OnChangeTeamCallback();
};
