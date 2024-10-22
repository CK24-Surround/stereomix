// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SMFaceControlComponent.generated.h"

class UTimelineComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STEREOMIX_API USMFaceControlComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USMFaceControlComponent();

	virtual void BeginPlay() override;

	virtual void Activate(bool bReset) override;

	virtual void Deactivate() override;

	void AddBlinkCurve(UCurveFloat* Curve);

	void AddMouthCurve(UCurveFloat* Curve);

	UPROPERTY(EditAnywhere, Category = "Design|Blink")
	int32 InEyeMaterialIndex = 0;

	UPROPERTY(EditAnywhere, Category = "Design|Blink")
	int32 OutEyeMaterialIndex = 0;

protected:
	void InitMaterials();

	static void UpdateValue(UMaterialInstanceDynamic* Material, FName ParameterName, float ParameterValue);

	UPROPERTY(VisibleAnywhere, Category = "Timeline")
	TObjectPtr<UTimelineComponent> BlinkTimelineComponent;

	UPROPERTY(VisibleAnywhere, Category = "Timeline")
	TObjectPtr<UTimelineComponent> MouthTimelineComponent;

	UPROPERTY(EditAnywhere, Category = "Design|Blink")
	FName BlinkParameterName = TEXT("Play");

	/** 눈을 깜빡이는데 필요한 프레임입니다. */
	UPROPERTY(EditAnywhere, Category = "Design|Blink")
	int32 BlinkFrameCount = 9;

	UPROPERTY(EditAnywhere, Category = "Design|Blink")
	float MinBlinkTime = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Design|Blink")
	float MaxBlinkTime = 3.0f;

	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> InEyeMID;

	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> OutEyeMID;
};
