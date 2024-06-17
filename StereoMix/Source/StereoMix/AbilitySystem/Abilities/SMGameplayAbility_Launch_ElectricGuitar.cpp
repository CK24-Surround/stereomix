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
	LaunchData.OffsetLocations.Empty();
	LaunchData.OffsetLocations.AddZeroed(ProjectileCount);
	LaunchData.Angles.Empty();
	LaunchData.Angles.AddZeroed(ProjectileCount);
	LaunchData.Count = 0;
	LaunchData.Rate = TotalLaunchTime / static_cast<float>(ProjectileCount - 1);

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