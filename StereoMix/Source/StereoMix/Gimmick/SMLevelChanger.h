// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/LevelStreaming.h"
#include "SMLevelChanger.generated.h"

UCLASS()
class STEREOMIX_API ASMLevelChanger : public AActor
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetLevelVisibility(FName LevelName, bool bVisible);

	UPROPERTY(EditAnywhere, Category = "Level")
	FName SubLevel1;

	UPROPERTY(EditAnywhere, Category = "Level")
	FName SubLevel2;

	UPROPERTY(EditAnywhere, Category = "Level")
	float SwitchInterval = 10.0f;

	UPROPERTY(EditAnywhere, Category = "Level")
	float LevelLifetime = 10.0f;

	float TimeSinceLastSwitch = 0.0f;

	uint8 bIsSubLevel1Active:1 = true;

private:
	void SwitchLevels();

	void SetLevelVisibility(FName LevelName, bool bVisibility);
};
