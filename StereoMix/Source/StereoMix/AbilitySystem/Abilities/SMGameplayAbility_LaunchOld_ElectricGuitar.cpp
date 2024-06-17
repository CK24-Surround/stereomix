// Copyright Surround, Inc. All Rights Reserved.


#include "SMGameplayAbility_LaunchOld_ElectricGuitar.h"

#include "Characters/SMPlayerCharacter.h"
#include "Games/SMGameMode.h"
#include "Projectiles/SMProjectile.h"
#include "Projectiles/SMProjectilePool.h"
#include "Utilities/SMLog.h"

class ASMGameMode;

void USMGameplayAbility_LaunchOld_ElectricGuitar::LaunchProjectile(const FVector& InStartLocation, const FVector& InNormal)
{
	LaunchData.OffsetLocations.Empty();
	LaunchData.OffsetLocations.AddZeroed(ProjectileCount);
	LaunchData.Angles.Empty();
	LaunchData.Angles.AddZeroed(ProjectileCount);
	LaunchData.Count = 0;
	LaunchData.Nomal = InNormal;

	const FVector RightVector = FVector::UpVector.Cross(LaunchData.Nomal);
	const float SegmentLength = ProjectileWidth / (ProjectileCount - 1);
	const FVector StartNormal = InNormal.RotateAngleAxis(-ProjectileTotalAngle / 2.0f, FVector::UpVector);
	const float AngleStep = ProjectileTotalAngle / (ProjectileCount - 1);
	TArray<FVector> Points;
	TArray<FVector> Angles;
	for (int32 i = 0; i < ProjectileCount; ++i)
	{
		Points.Add(RightVector * (i * SegmentLength));
		Angles.Add(StartNormal.RotateAngleAxis(AngleStep * i, FVector::UpVector));
	}

	const int32 MidIndex = Points.Num() / 2;
	LaunchData.OffsetLocations[0] = Points[MidIndex];
	LaunchData.Angles[0] = Angles[MidIndex];

	LaunchData.CenterOffsetLocation = -RightVector * (ProjectileWidth / 2.0);

	int32 Left = MidIndex - 1;
	int32 Right = MidIndex + 1;
	int32 Index = 1;

	while (Left >= 0 || Right < ProjectileCount)
	{
		if (Left >= 0)
		{
			LaunchData.OffsetLocations[Index] = Points[Left];
			LaunchData.Angles[Index] = Angles[Left];
			++Index;
			--Left;
		}

		if (Right < ProjectileCount)
		{
			LaunchData.OffsetLocations[Index] = Points[Right];
			LaunchData.Angles[Index] = Angles[Right];
			++Index;
			++Right;
		}
	}

	LaunchData.Rate = TotalLaunchTime / static_cast<float>(ProjectileCount - 1);

	// 첫 발은 즉시 발사합니다.
	LaunchTimerCallback();
}

void USMGameplayAbility_LaunchOld_ElectricGuitar::LaunchTimerCallback()
{
	ASMPlayerCharacter* SourceCharacter = GetSMPlayerCharacterFromActorInfo();
	if (!SourceCharacter)
	{
		EndAbilityByCancel();
		return;
	}

	ASMGameMode* CachedSMGameMode = GetWorld()->GetAuthGameMode<ASMGameMode>();
	if (!ensureAlways(CachedSMGameMode))
	{
		EndAbilityByCancel();
		return;
	}

	const ESMTeam SourceTeam = SourceCharacter->GetTeam();
	if (SourceTeam == ESMTeam::None)
	{
		EndAbilityByCancel();
		return;
	}

	USMProjectilePool* ProjectilePool = CachedSMGameMode->GetEletricGuitarProjectilePool(SourceTeam);
	if (!ensureAlways(ProjectilePool))
	{
		EndAbilityByCancel();
		return;
	}

	ASMProjectile* NewProjectile = ProjectilePool->GetProjectile();
	if (!NewProjectile)
	{
		EndAbilityByCancel();
		return;
	}

	const FVector NewStartLocation = SourceCharacter->GetActorLocation() + LaunchData.CenterOffsetLocation;

	// 투사체를 발사합니다.
	NewProjectile->Launch(SourceCharacter, NewStartLocation + LaunchData.OffsetLocations[LaunchData.Count], LaunchData.Angles[LaunchData.Count], ProjectileSpeed, MaxDistance, Damage);
	++LaunchData.Count;

	ExecuteLaunchFX(LaunchData.Count);

	if (LaunchData.Count < ProjectileCount)
	{
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &USMGameplayAbility_LaunchOld_ElectricGuitar::LaunchTimerCallback, LaunchData.Rate, false);
	}
	else
	{
		K2_EndAbility();
	}
}