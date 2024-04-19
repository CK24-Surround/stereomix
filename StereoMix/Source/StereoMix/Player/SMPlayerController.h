// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SMPlayerController.generated.h"

class USMUserWidget_VictoryDefeat;
class USMUserWidget_HUD;
class USMControlData;
/**
 * 
 */
UCLASS()
class STEREOMIX_API ASMPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ASMPlayerController();

protected:
	virtual void BeginPlay() override;

protected:
	virtual void OnRep_PlayerState() override;


protected:
	void InitControl();

public:
	FORCEINLINE const USMControlData* GetControlData() const { return ControlData; }

protected:
	UPROPERTY()
	TObjectPtr<const USMControlData> ControlData;

protected:
	UPROPERTY(EditAnywhere, Category = "UI|HUD")
	TSubclassOf<USMUserWidget_HUD> HUDWidgetClass;

	UPROPERTY()
	TObjectPtr<USMUserWidget_HUD> HUDWidget;

	UPROPERTY(EditAnywhere, Category = "UI|HUD")
	TSubclassOf<USMUserWidget_VictoryDefeat> VictoryDefeatWidgetClass;

	UPROPERTY()
	TObjectPtr<USMUserWidget_VictoryDefeat> VictoryDefeatWidget;
};
