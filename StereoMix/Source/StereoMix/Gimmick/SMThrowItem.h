#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SMThrowItem.generated.h"

class ASMItemBase;
class ASMThrowableItem;

UCLASS(Abstract)
class STEREOMIX_API ASMThrowItem : public AActor
{
	GENERATED_BODY()

public:
	ASMThrowItem();

protected:
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

public:
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

	void ThrowItem();

protected:
	void InternalThrowItem();

	UPROPERTY(VisibleAnywhere, Category = "Root")
	TObjectPtr<USceneComponent> SceneComponent;

private:
	FTimerHandle ThrowTimerHandle;
};
