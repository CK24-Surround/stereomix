#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SMThrowItem.generated.h"

class ASMHoldableItemBase;

UCLASS()
class STEREOMIX_API ASMThrowItem : public AActor
{
	GENERATED_BODY()

public:
	ASMThrowItem();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gimmick")
	TSubclassOf<ASMHoldableItemBase> ItemToThrow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gimmick")
	float ParabolaHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gimmick")
	float LandingTime;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gimmick")
	float ThrowInterval;

	UPROPERTY(Replicated)
	TArray<FVector> LaunchVelocity;
		
	UPROPERTY(Replicated)
	TArray<FVector> InitialLocation;
	
	UPROPERTY(Replicated)
	TArray<double> ThrowStartTime;

	UFUNCTION(Server, Reliable)
	void ServerThrowItem();

private:
	FTimerHandle ThrowTimerHandle;

	UPROPERTY(Replicated)
	TArray<TObjectPtr<ASMHoldableItemBase>> SpawnedItem;
};
