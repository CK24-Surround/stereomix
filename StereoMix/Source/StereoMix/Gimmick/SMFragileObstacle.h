// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/SMDamageInterface.h"
#include "SMFragileObstacle.generated.h"

class UBoxComponent;

UCLASS(Abstract)
class STEREOMIX_API ASMFragileObstacle : public AActor, public ISMDamageInterface
{
	GENERATED_BODY()

public:
	ASMFragileObstacle();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void BeginPlay() override;

	/** 장애물을 처음 상태로 초기화할 때 호출됩니다. 서버에서만 호출해야합니다. */
	void InitObstacle();

	virtual AActor* GetLastAttacker() const override { return LastAttacker.Get(); }

	virtual void SetLastAttacker(AActor* NewAttacker) override { LastAttacker = NewAttacker; }

	virtual void ReceiveDamage(AActor* NewAttacker, float InDamageAmount) override;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Root")
	TObjectPtr<USceneComponent> SceneComponent;

	UPROPERTY(VisibleAnywhere, Category = "Collider")
	TObjectPtr<UBoxComponent> ColliderComponent;

	UPROPERTY(VisibleAnywhere, Category = "Visual")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated, Category = "Design")
	float CurrentDurability = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated, Category = "Design")
	float MaxDurability = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Design")
	TArray<float> HealthThresholds;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Design")
	TArray<TObjectPtr<UStaticMesh>> HealthThresholdMeshes;

	UPROPERTY()
	TObjectPtr<UStaticMesh> InitialMesh;

	UPROPERTY(Replicated)
	TWeakObjectPtr<AActor> LastAttacker;

private:
	UFUNCTION(NetMulticast, Reliable)
	void UpdateColliderProfile(FName InProfileName);

	UFUNCTION(NetMulticast, Reliable)
	void UpdateMeshBasedOnHealth(float InHealth);
};
