#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SMThrowItem.generated.h"

class ASMTile;
class ASMItemBase;
class ASMThrowableItem;

UCLASS(Abstract)
class STEREOMIX_API ASMThrowItem : public AActor
{
	GENERATED_BODY()

public:
	ASMThrowItem();

	/** 아이템을 생성할 수 있는 위치를 초기화 후 다시 찾습니다. */
	void UpdateAvailableSpawnLocations();

protected:
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

	static FVector CalculateHalfExtent(int32 Columns, int32 Rows, float Height);

	static FVector GetBoxCenter(const FVector& ActorLocation, float ZOffset);

	static FVector CalculateSpawnLocation(const FVector& TileLocation, float Z);

	bool IsLocationAvailableForSpawn(const FVector& Location) const;

	void ScheduleThrowItem();

	void ThrowItem();

	void InternalThrowItem(const FVector& TargetLocation);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Design")
	TArray<TSubclassOf<ASMItemBase>> ThrowingItems;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Design")
	TSubclassOf<ASMThrowableItem> ThrowableItemClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Design")
	int32 ThrowCount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Design")
	float ThrowInterval = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Design")
	int32 MaxThrowTilesRow = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Design")
	int32 MaxThrowTilesColumn = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Design")
	float ParabolaHeight = 1000.0f;

	UPROPERTY(VisibleAnywhere, Category = "Root")
	TObjectPtr<USceneComponent> SceneComponent;

private:
	TArray<FVector> AvailableSpawnLocations;
};
