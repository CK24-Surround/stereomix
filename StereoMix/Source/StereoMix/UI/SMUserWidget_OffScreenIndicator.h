// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMUserWidget.h"
#include "SMUserWidget_OffScreenIndicator.generated.h"

class UScaleBox;

/**
 * 
 */
UCLASS()
class STEREOMIX_API USMUserWidget_OffScreenIndicator : public USMUserWidget
{
	GENERATED_BODY()

public:
	USMUserWidget_OffScreenIndicator();

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
	/** 인디케이터가 표시되야할 스크린 좌표를 반환합니다. */
	FVector2D GetOffScreenIndicatorScreenLocation(const FGeometry& InGeometry, const FVector& TargetLocation);

	/** 만약 카메라 뒤로 타겟이 넘어가는 경우 ProjectWorldLocationToScreen를 사용할 수 없어 수동으로 계산해야합니다. 이를 위한 함수입니다.*/
	FVector2D GetScreenLocation(const FVector& TargetLocation);

protected:
	UPROPERTY(meta = (BindWidget = "Base"))
	TObjectPtr<UScaleBox> Base;

	/**
	 * 경계면을 기준으로 인디케이터가 나타나는 위치의 오프셋의 크기를 결정하는데 사용됩니다. 이 값은 뷰포트 크기의 세로사이즈를 나누는 값으로 쓰이며 이 나눈 값을 오프셋을 사용합니다.
	 * 결과적으로 작은 값을 쓸수록 오프셋이 커집니다.
	 */
	UPROPERTY(EditAnywhere, Category = "Design")
	float VerticalOffsetRatio = 10.0f;
};
