// Copyright Surround, Inc. All Rights Reserved.


#include "SMUserWidget_OffScreenIndicator.h"

#include "Blueprint/SlateBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/ScaleBox.h"
#include "Utilities/SMLog.h"

USMUserWidget_OffScreenIndicator::USMUserWidget_OffScreenIndicator() {}

void USMUserWidget_OffScreenIndicator::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UCanvasPanelSlot* BaseSlot = Cast<UCanvasPanelSlot>(Base->Slot);
	if (!ensureAlways(BaseSlot))
	{
		return;
	}

	APlayerController* SourcePlayerController = GetOwningPlayer();
	if (!ensureAlways(SourcePlayerController))
	{
		return;
	}

	const FVector TargetLocation = FVector(0.0, 0.0, 100.0);
	const FVector2D TargetOffScreendIndicatorScreenLocation = GetOffScreenIndicatorScreenLocation(MyGeometry, TargetLocation);

	// 스크린 좌표를 위젯에 적용하기 위해 위젯의 로컬 좌표로 변환하고 적용합니다.
	FVector2D LocalCoordinate;
	USlateBlueprintLibrary::ScreenToWidgetLocal(SourcePlayerController, MyGeometry, TargetOffScreendIndicatorScreenLocation, LocalCoordinate);
	BaseSlot->SetPosition(LocalCoordinate);
}

FVector2D USMUserWidget_OffScreenIndicator::GetOffScreenIndicatorScreenLocation(const FGeometry& InGeometry, const FVector& TargetLocation)
{
	APlayerController* SourcePlayerController = GetOwningPlayer();
	if (!ensureAlways(SourcePlayerController))
	{
		return FVector2D();
	}

	const FVector2D ViewportSize = UWidgetLayoutLibrary::GetViewportSize(SourcePlayerController);
	const FVector2D ScreenCenter(ViewportSize.X / 2.0, ViewportSize.Y / 2.0);
	const float ViewportOffset = ViewportSize.Y / VerticalOffsetRatio;

	FVector2D TargetScreenLocation;
	// 월드 좌표를 스크린좌표로 변환합니다. 만약에 카메라 뒤에 위치하는 경우 수동으로 계산해줍니다.
	if (!SourcePlayerController->ProjectWorldLocationToScreen(TargetLocation, TargetScreenLocation, true))
	{
		TargetScreenLocation = GetScreenLocation(TargetLocation);
	}

	// 스크린좌표의 중심에서 타겟을 향한 방향과 타겟의 위치를 저장합니다.
	FVector2D Direction = (TargetScreenLocation - ScreenCenter).GetSafeNormal();
	FVector2D ClampedTargetScreenLocation = TargetScreenLocation;

	// 오프셋을 고려하여 경계면을 설정합니다.
	FVector2D MinBoundary = FVector2D(ViewportOffset, ViewportOffset);
	FVector2D MaxBoundary = FVector2D(ViewportSize.X - ViewportOffset, ViewportSize.Y - ViewportOffset);

	// 타겟이 화면 밖을 벗어났는지 확인합니다. 타겟이 화면을 벗어난 경우에는 경계면에 올바르게 UI가 출력되도록 처리합니다.
	bool bIsOutOfBounds = TargetScreenLocation.X < MinBoundary.X || TargetScreenLocation.X > MaxBoundary.X || TargetScreenLocation.Y < MinBoundary.Y || TargetScreenLocation.Y > MaxBoundary.Y;
	if (bIsOutOfBounds)
	{
		// 화면 경계와의 교점 계산하기 위한 배열입니다.
		TArray<FVector2D> Intersections;

		// 방향이 좌측을 향한 경우입니다.
		if (Direction.X < 0)
		{
			// 중심을 기준으로 왼쪽 경계까지의 길이를 구한 뒤 방향의 x로 나눕니다. 이를 통해 어떤 길이를 가질때 왼쪽 경계에 도달할 수 있는지 구할 수 있습니다.
			float T = (MinBoundary.X - ScreenCenter.X) / Direction.X;
			// 중심으로부터 타겟을 방향으로 위에서 구한 길이만큼 곱하면 왼쪽 경계에 도달하는 선을 구할 수 있습니다.
			FVector2D Intersection = ScreenCenter + Direction * T;
			// 이 선의 Y값이 상단과 하단 경계 사이에 속한다면 유효한 경계내에 있는 것을 의미하므로 값을 저장해둡니다.
			if (Intersection.Y >= MinBoundary.Y && Intersection.Y <= MaxBoundary.Y)
			{
				Intersections.Add(Intersection);
			}
		}

		// 방향이 우측을 향한 경우입니다.
		if (Direction.X > 0)
		{
			float T = (MaxBoundary.X - ScreenCenter.X) / Direction.X;
			FVector2D Intersection = ScreenCenter + Direction * T;
			if (Intersection.Y >= MinBoundary.Y && Intersection.Y <= MaxBoundary.Y)
			{
				Intersections.Add(Intersection);
			}
		}

		// 방향이 상단을 향한 경우입니다. (스크린 좌표이기 때문에 음수인 경우 상단입니다.)
		if (Direction.Y < 0)
		{
			float T = (MinBoundary.Y - ScreenCenter.Y) / Direction.Y;
			FVector2D Intersection = ScreenCenter + Direction * T;
			if (Intersection.X >= MinBoundary.X && Intersection.X <= MaxBoundary.X)
			{
				Intersections.Add(Intersection);
			}
		}

		// 방향이 하단을 향한 경우입니다.
		if (Direction.Y > 0)
		{
			float T = (MaxBoundary.Y - ScreenCenter.Y) / Direction.Y;
			FVector2D Intersection = ScreenCenter + Direction * T;
			if (Intersection.X >= MinBoundary.X && Intersection.X <= MaxBoundary.X)
			{
				Intersections.Add(Intersection);
			}
		}

		// 가장 가까운 교점 선택합니다.
		// 이해를 돕기 위한 예시입니다.
		// 위의 식에서 좌측 경계면에서 절반보다 위쪽에 인디케이터가 찍혀야하는 경우 if문에 Direction.X < 0와 Direction.Y < 0가 ture가 됩니다.
		// 이 때 교점의 거리가 Direction.Y < 0인 경우는 굉장히 멀게 찍혀 좌측 경계면을 넘어 버리게 될 것입니다. 이처럼 대부분의 경우 Intersections는 1개의 값만 갖게됩니다.
		// 그렇다면 조건자체를 경계면을 넘는 쪽만 계산하는게 더 좋다고 생각할 수 있습니다. 하지만 문제가 생기게됩니다. 확률은 매우 낮지만 두가지 경계면에 걸쳐 있는 경우가 존재할 수 있습니다.
		// 이 경우 올바른 처리가 되지 않습니다. 이런 상황을 방지하고자 아래 수식을 추가했습니다.
		float MinDistanceSqaured = FLT_MAX;
		for (const FVector2D& Intersection : Intersections)
		{
			float DistanceSqaured = FVector2D::DistSquared(ScreenCenter, Intersection);
			if (DistanceSqaured < MinDistanceSqaured)
			{
				MinDistanceSqaured = DistanceSqaured;
				ClampedTargetScreenLocation = Intersection;
			}
		}
	}

	return ClampedTargetScreenLocation;
}

FVector2D USMUserWidget_OffScreenIndicator::GetScreenLocation(const FVector& TargetLocation)
{
	APlayerController* SourcePlayerController = GetOwningPlayer();
	if (!ensureAlways(SourcePlayerController))
	{
		return FVector2D::ZeroVector;
	}

	const FVector2D ViewportSize = UWidgetLayoutLibrary::GetViewportSize(SourcePlayerController);
	const FVector2D ScreenCenter(ViewportSize.X / 2.0f, ViewportSize.Y / 2.0f);

	FVector CameraLocation;
	FRotator CameraRotation;
	SourcePlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);

	FVector ToTarget = TargetLocation - CameraLocation;
	ToTarget = ToTarget.GetSafeNormal();

	const FVector Forward = CameraRotation.Vector();
	const FVector Right = FRotationMatrix(CameraRotation).GetUnitAxis(EAxis::Y);
	const FVector Up = FRotationMatrix(CameraRotation).GetUnitAxis(EAxis::Z);

	const float ForwardDot = FVector::DotProduct(Forward, ToTarget);
	const float RightDot = FVector::DotProduct(Right, ToTarget);
	const float UpDot = FVector::DotProduct(Up, ToTarget);

	FVector2D ScreenLocation;
	if (ForwardDot <= 0.0f)
	{
		// 카메라의 뒤쪽에 있는 경우, 반대 방향으로 스크린 경계에 배치
		FVector2D OffScreenDirection = FVector2D(RightDot, -UpDot).GetSafeNormal();
		ScreenLocation = ScreenCenter + OffScreenDirection * ScreenCenter.X;

		return ScreenLocation;
	}
	else
	{
		ScreenLocation.X = ScreenCenter.X + (RightDot / ForwardDot) * ScreenCenter.X;
		ScreenLocation.Y = ScreenCenter.Y - (UpDot / ForwardDot) * ScreenCenter.Y;

		return ScreenLocation;
	}
}
