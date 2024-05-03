// Copyright Surround, Inc. All Rights Reserved.


#include "SMCalculateBlueprintLibrary.h"

FVector USMCalculateBlueprintLibrary::SuggestProjectileVelocity_CustomApexHeight(const UObject* WorldContextObject, const FVector& StartLocation, const FVector& EndLocation, float ApexHeight, float OverrideGravityZ)
{
	if (!ensureAlways(GEngine))
	{
		return FVector::ZeroVector;
	}

	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!ensureAlways(World))
	{
		return FVector::ZeroVector;
	}

	const FVector StartToEnd = EndLocation - StartLocation;
	const float StartToEndDistance = StartToEnd.Size();

	const float GravityZ = FMath::IsNearlyEqual(OverrideGravityZ, 0.0f) ? World->GetGravityZ() : OverrideGravityZ;

	const float TimeToApex = FMath::Sqrt(2 * ApexHeight / -GravityZ);
	const float VerticalVelocity = -GravityZ * TimeToApex;

	if (StartToEndDistance <= UE_KINDA_SMALL_NUMBER)
	{
		return FVector(0.0, 0.0, VerticalVelocity);
	}

	const float HorizontalVelocity = StartToEnd.Size2D() / (2 * TimeToApex);

	FVector HorizontalDirection = StartToEnd.GetSafeNormal2D();
	const FVector LaunchVelocity = FVector(HorizontalDirection.X * HorizontalVelocity, HorizontalDirection.Y * HorizontalVelocity, VerticalVelocity);

	return LaunchVelocity;
}

FVector USMCalculateBlueprintLibrary::SuggestProjectileVelocity_CustomApexHeight2(const UObject* WorldContextObject, const FVector& StartLocation, const FVector& EndLocation, float ApexHeight, float OverrideGravityZ)
{
	if (!ensureAlways(GEngine))
	{
		return FVector::ZeroVector;
	}

	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!ensureAlways(World))
	{
		return FVector::ZeroVector;
	}

	// 중력을 구합니다.
	const float GravityZ = FMath::IsNearlyZero(OverrideGravityZ) ? World->GetGravityZ() : OverrideGravityZ;

	// 원하는 ApexHeight까지 도달하기 위한 수직 속도를 구합니다.
	const float InitialVelocityZ = FMath::Sqrt(2 * -GravityZ * (ApexHeight - StartLocation.Z));

	// 최고점까지 시간은 중력가속도로 나누어 몇초가 걸리는지 계산합니다.
	const float TimeToApex = InitialVelocityZ / -GravityZ;

	// 최고점부터 하강 시간을 다시 계산합니다. 최고점부터 바닥까지의 거리를 기준으로 자유낙하 운동을 계산합니다.
	const float DescentTime = FMath::Sqrt(2 * (ApexHeight - EndLocation.Z) / -GravityZ);
	// 최고점까지 시간과 하강 시간을 더해 최종 체공 시간을 구합니다.
	const float TotalTime = TimeToApex + DescentTime;

	// 시작지점부터 끝지점까지의 수평 거리를 구합니다.
	const FVector Delta = EndLocation - StartLocation;
	const float HorizontalDistance = Delta.Size2D();

	// 구해진 수평 거리에 최종 체공 시간으로 나누어 속력을구합니다. 수평이동은 중력에 영향을 받지 않기때문에 일정합니다.
	const float HorizontalVelocity = HorizontalDistance / TotalTime;

	// 수평 방향을 구합니다.
	const FVector HorizontalDirection = Delta.GetSafeNormal2D();

	// 위의 값들을 통해 수평 속도, 수직 속도를 더해 최종 속도를 구합니다.
	const FVector LaunchVelocity = HorizontalDirection * HorizontalVelocity + FVector(0, 0, InitialVelocityZ);

	return LaunchVelocity;
}
