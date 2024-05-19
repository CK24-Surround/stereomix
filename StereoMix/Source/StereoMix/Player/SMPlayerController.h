// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SMPlayerController.generated.h"

class USMUserWidget_ScreenIndicator;
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

	/** 타겟을 향하는 인디케이터를 추가합니다. */
	void AddScreendIndicator(AActor* TargetActor);

	/** 타겟을 향하는 인디케이터를 제거합니다. */
	void RemoveScreenIndicator(AActor* TargetActor);

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

	UPROPERTY(EditAnywhere, Category = "UI|OffScreenIndicator")
	TSubclassOf<USMUserWidget_ScreenIndicator> OffScreenIndicatorClass;

	UPROPERTY()
	TMap<AActor*, TObjectPtr<USMUserWidget_ScreenIndicator>> OffScreenIndicators;
};
