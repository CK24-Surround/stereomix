#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SMThrowItem.generated.h"

class ASMThrowableItem;

UCLASS()
class STEREOMIX_API ASMThrowItem : public AActor
{
	GENERATED_BODY()

public:
	ASMThrowItem();

protected:
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gimmick")
	TSubclassOf<ASMThrowableItem> ItemToThrow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gimmick")
	int32 ThrowCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gimmick")
	float ThrowInterval;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gimmick")
	float TileSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gimmick")
	int32 MaxThrowTilesRow;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gimmick")
	int32 MaxThrowTilesColumn;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gimmick")
	float ParabolaHeight;
	
	UFUNCTION(Server, Reliable)
	void ServerThrowItem();

	void ThrowItem();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Root")
	TObjectPtr<USceneComponent> SceneComponent;
	
private:
	FTimerHandle ThrowTimerHandle;
};
