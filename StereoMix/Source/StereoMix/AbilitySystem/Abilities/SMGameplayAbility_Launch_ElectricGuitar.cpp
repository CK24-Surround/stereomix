// Copyright Surround, Inc. All Rights Reserved.


#include "SMGameplayAbility_Launch_ElectricGuitar.h"

#include "GameplayCueManager.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "Characters/SMPlayerCharacter.h"
#include "Games/SMGameMode.h"
#include "Projectiles/SMProjectile.h"
#include "Projectiles/SMProjectilePool.h"
#include "Utilities/SMLog.h"

void USMGameplayAbility_Launch_ElectricGuitar::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ASMPlayerCharacter* SourceCharacter = GetSMPlayerCharacterFromActorInfo();
	if (!SourceCharacter)
	{
		NET_LOG(nullptr, Warning, TEXT("소스 캐릭터가 무효합니다."))
		return;
	}

	USMAbilitySystemComponent* SourceASC = GetSMAbilitySystemComponentFromActorInfo();
	if (!ensureAlways(SourceASC))
	{
		EndAbilityByCancel();
		return;
	}

	if (IsLocallyControlled())
	{
		LaunchProjectile();
	}
}

void USMGameplayAbility_Launch_ElectricGuitar::LaunchProjectile()
{
	LaunchData.SpawnLocationOffsets.Empty();
	LaunchData.SpawnLocationOffsets.AddZeroed(ProjectileCount); // 발사해야할 투사체의 개수만큼 사이즈를 초기화하고 0으로 채웁니다.
	LaunchData.SpawnRotationOffsets.Empty();
	LaunchData.SpawnRotationOffsets.AddZeroed(ProjectileCount); // 발사해야할 투사체의 개수만큼 사이즈를 초기화하고 0으로 채웁니다.
	LaunchData.Count = 0;
	LaunchData.Rate = TotalLaunchTime / static_cast<float>(ProjectileCount - 1);

	// TODO: 디테일 로직
	// const FVector StartLocation = FVector::LeftVector * (ProjectileWidth / 2.0f);
	// const float SegmentLength = ProjectileWidth / (ProjectileCount - 1);
	//
	// const FVector StartNormal = InNormal.RotateAngleAxis(-ProjectileTotalAngle / 2.0f, FVector::UpVector);
	// const float AngleStep = ProjectileTotalAngle / (ProjectileCount - 1);
	//
	// TArray<FVector> Points;
	// TArray<FVector> Angles;
	// for (int32 i = 0; i < ProjectileCount; ++i)
	// {
	// 	Points.Add(StartLocation * (i * SegmentLength));
	// 	Angles.Add(StartNormal.RotateAngleAxis(AngleStep * i, FVector::UpVector));
	// }
	//
	// const int32 MidIndex = Points.Num() / 2;
	// LaunchData.OffsetLocations[0] = Points[MidIndex];
	// LaunchData.Angles[0] = Angles[MidIndex];
	//
	// LaunchData.CenterOffsetLocation = -StartLocation * (ProjectileWidth / 2.0);
	//
	// int32 Left = MidIndex - 1;
	// int32 Right = MidIndex + 1;
	// int32 Index = 1;
	//
	// while (Left >= 0 || Right < ProjectileCount)
	// {
	// 	if (Left >= 0)
	// 	{
	// 		LaunchData.OffsetLocations[Index] = Points[Left];
	// 		LaunchData.Angles[Index] = Angles[Left];
	// 		++Index;
	// 		--Left;
	// 	}
	//
	// 	if (Right < ProjectileCount)
	// 	{
	// 		LaunchData.OffsetLocations[Index] = Points[Right];
	// 		LaunchData.Angles[Index] = Angles[Right];
	// 		++Index;
	// 		++Right;
	// 	}
	// }
	// TODO: 디테일 로직

	LaunchProjectileTimerCallback();
}

void USMGameplayAbility_Launch_ElectricGuitar::LaunchProjectileTimerCallback()
{
	ASMPlayerCharacter* SourceCharacter = GetSMPlayerCharacterFromActorInfo();
	if (!SourceCharacter)
	{
		NET_LOG(nullptr, Warning, TEXT("소스 캐릭터가 무효합니다."))
		return;
	}

	// 투사체를 발사합니다.
	const FVector SourceLocation = SourceCharacter->GetActorLocation();
	const FVector TargetLocatiion = SourceCharacter->GetCursorTargetingPoint();
	const FVector Normal = (TargetLocatiion - SourceLocation).GetSafeNormal2D();
	ServerRPCLaunchProjectile(SourceLocation, Normal, ++LaunchData.Count);

	if (LaunchData.Count < ProjectileCount)
	{
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &USMGameplayAbility_Launch_ElectricGuitar::LaunchProjectileTimerCallback, LaunchData.Rate);
	}
	else
	{
		K2_EndAbility();
	}
}

void USMGameplayAbility_Launch_ElectricGuitar::ServerRPCLaunchProjectile_Implementation(const FVector_NetQuantize10& SourceLocation, const FVector_NetQuantizeNormal& Normal, int LaunchCount)
{
	ASMPlayerCharacter* SourceCharacter = GetSMPlayerCharacterFromActorInfo();
	if (!SourceCharacter)
	{
		EndAbilityByCancel();
		return;
	}

	USMAbilitySystemComponent* SourceASC = GetSMAbilitySystemComponentFromActorInfo();
	if (!ensureAlways(SourceASC))
	{
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

	// 투사체를 발사합니다.
	NewProjectile->Launch(SourceCharacter, SourceLocation, Normal, ProjectileSpeed, MaxDistance, Damage);

	// FX를 실행합니다.
	FGameplayCueParameters GCParams;
	GCParams.TargetAttachComponent = SourceCharacter->GetRootComponent();
	GCParams.RawMagnitude = LaunchCount;
	SourceASC->ExecuteGameplayCue(SMTags::GameplayCue::ProjectileLaunch, GCParams);
}