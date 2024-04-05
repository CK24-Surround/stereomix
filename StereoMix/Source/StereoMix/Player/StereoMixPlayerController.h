// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "StereoMixPlayerController.generated.h"

class UStereoMixControlData;
/**
 * 
 */
UCLASS()
class STEREOMIX_API AStereoMixPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AStereoMixPlayerController();

protected:
	virtual void BeginPlay() override;

protected:
	void InitControl();

public:
	FORCEINLINE const UStereoMixControlData* GetControlData() const { return ControlData; }

protected:
	UPROPERTY()
	TObjectPtr<const UStereoMixControlData> ControlData;
};
