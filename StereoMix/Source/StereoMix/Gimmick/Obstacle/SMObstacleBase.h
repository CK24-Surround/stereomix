// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SMObstacleBase.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;
class UBoxComponent;

UCLASS(Abstract)
class STEREOMIX_API ASMObstacleBase : public AActor
{
	GENERATED_BODY()

public:
	ASMObstacleBase();

	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;

	virtual void SetCollisionEnabled(bool bNewIsCollisionEnabled);

protected:
	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetCollisionEnabled(bool bNewIsCollisionEnabled);

	UFUNCTION(NetMulticast, Reliable)
	void ClientSetMeshAndNiagaraSystem(UStaticMesh* NewMesh, UNiagaraSystem* NewNiagaraSystem);

	UPROPERTY(VisibleAnywhere, Category = "Collider")
	TObjectPtr<UBoxComponent> ColliderComponent;

	UPROPERTY(VisibleAnywhere, Category = "Visual")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(VisibleAnywhere, Category = "Visual")
	TObjectPtr<UNiagaraComponent> NiagaraComponent;

	UPROPERTY(EditAnywhere, Category = "Design")
	float SpawnDelay = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Design")
	float SpawnEffectDuration = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Design")
	TObjectPtr<UNiagaraSystem> DelayEffect;

	UPROPERTY(EditAnywhere, Category = "Design")
	TObjectPtr<UNiagaraSystem> SpawnEffect;

	UPROPERTY()
	TObjectPtr<UStaticMesh> OriginMesh;

	UPROPERTY()
	TObjectPtr<UNiagaraSystem> OriginNiagaraSystem;
};
