// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/SMDamageInterface.h"
#include "SMFragileObstacle.generated.h"

class UBoxComponent;

USTRUCT(BlueprintType)
struct FSMFragileObstacleDurabilityThresholdData
{
	GENERATED_BODY()

	float DurabilityRatio;

	UPROPERTY()
	TObjectPtr<UStaticMesh> Mesh;
};

UCLASS(Abstract)
class STEREOMIX_API ASMFragileObstacle : public AActor, public ISMDamageInterface
{
	GENERATED_BODY()

public:
	ASMFragileObstacle();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PostInitializeComponents() override;

	/** 장애물을 처음 상태로 복구할때 사용합니다. */
	UFUNCTION(Server, Reliable)
	void ServerRestoreObstacle();

	UFUNCTION(Server, Reliable)
	void ServerSetCurrentDurability(float NewCurrentDurability);

	UFUNCTION(Server, Reliable)
	void ServerSetMaxDurability(float NewMaxDurability);

	virtual AActor* GetLastAttacker() const override { return LastAttacker.Get(); }

	virtual void SetLastAttacker(AActor* NewAttacker) override { LastAttacker = NewAttacker; }

	virtual void ReceiveDamage(AActor* NewAttacker, float InDamageAmount) override;

protected:
	UFUNCTION()
	void OnRep_CurrentDurability();

	UFUNCTION()
	void OnRep_MaxDurability();

	UPROPERTY(VisibleAnywhere, Category = "Root")
	TObjectPtr<USceneComponent> SceneComponent;

	UPROPERTY(VisibleAnywhere, Category = "Collider")
	TObjectPtr<UBoxComponent> ColliderComponent;

	UPROPERTY(VisibleAnywhere, Category = "Visual")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = "OnRep_CurrentDurability", Category = "Design")
	float CurrentDurability = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = "OnRep_MaxDurability", Category = "Design")
	float MaxDurability = 100.0f;

	/** 좌측에는 퍼센트를 정규화한 값을 넣고, 우측에는 해당하는 메시를 넣어주세요. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Design")
	TArray<FSMFragileObstacleDurabilityThresholdData> DurabilityThresholds;

	UPROPERTY()
	TObjectPtr<UStaticMesh> OriginalMesh;

	UPROPERTY(Replicated)
	TWeakObjectPtr<AActor> LastAttacker;

private:
	void SetCollisionEnabled(bool bNewIsCollisionEnabled);
	
	void UpdateMeshBasedOnDurability();
};
