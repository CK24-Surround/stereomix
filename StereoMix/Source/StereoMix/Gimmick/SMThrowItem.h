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

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gimmick")
	TSubclassOf<ASMThrowableItem> ItemToThrow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gimmick")
	float ParabolaHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gimmick")
	float LandingTime;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gimmick")
	float ThrowInterval;
	
	UFUNCTION(Server, Reliable)
	void ServerThrowItem();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Root")
	TObjectPtr<USceneComponent> SceneComponent;
	
private:
	FTimerHandle ThrowTimerHandle;
};
