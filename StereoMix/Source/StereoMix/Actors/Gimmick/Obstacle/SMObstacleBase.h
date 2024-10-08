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

	virtual void UnloadObstacle();

	FORCEINLINE float GetDestroyEffectDuration() const { return DestroyEffectDuration; }

protected:
	UFUNCTION(NetMulticast, Reliable)
	void MulticastPushBack();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetCollisionEnabled(bool bNewIsCollisionEnabled);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetMeshAndNiagaraSystem(UStaticMesh* NewMesh, UNiagaraSystem* NewNiagaraSystem);

	UPROPERTY(VisibleAnywhere, Category = "Collider")
	TObjectPtr<UBoxComponent> ColliderComponent;

	UPROPERTY(VisibleAnywhere, Category = "Visual")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(VisibleAnywhere, Category = "Visual")
	TObjectPtr<UNiagaraComponent> NiagaraComponent;

	UPROPERTY(EditAnywhere, Category = "Design")
	uint8 bSpawnImmediately:1 = true;

	UPROPERTY(EditAnywhere, Category = "Design", meta = (ClampMin = "0.0"))
	float SpawnPushBackMagnitude = 700.0f;

	UPROPERTY(EditAnywhere, Category = "Design", meta = (ClampMin = "0.0"))
	float SpawnDelay = 3.0f;

	UPROPERTY(EditAnywhere, Category = "Design", meta = (ClampMin = "0.01"))
	float PreSpawnEffectDuration = 0.01f;

	UPROPERTY(EditAnywhere, Category = "Design", meta = (ClampMin = "0.01"))
	float SpawnEffectDuration = 0.01f;

	UPROPERTY(EditAnywhere, Category = "Design", meta = (ClampMin = "0.01"))
	float DestroyEffectDuration = 0.01f;

	UPROPERTY(EditAnywhere, Category = "Design")
	TObjectPtr<UNiagaraSystem> PreSpawnEffect;

	UPROPERTY(EditAnywhere, Category = "Design")
	TObjectPtr<UNiagaraSystem> DestroyEffect;

	UPROPERTY()
	TObjectPtr<UStaticMesh> OriginalMesh;

	UPROPERTY()
	TObjectPtr<UNiagaraSystem> OriginalNiagaraSystem;
};
