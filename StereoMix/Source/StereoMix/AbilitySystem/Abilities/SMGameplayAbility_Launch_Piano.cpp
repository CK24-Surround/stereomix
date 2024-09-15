// Copyright Surround, Inc. All Rights Reserved.


#include "SMGameplayAbility_Launch_Piano.h"

#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "Characters/SMPlayerCharacter.h"
#include "Games/SMGameMode.h"
#include "Projectiles/SMProjectile.h"
#include "Utilities/SMLog.h"

void USMGameplayAbility_Launch_Piano::LaunchProjectile(const FVector& InStartLocation, const FVector& InNormal)
{
	ASMPlayerCharacter* SourceCharacter = GetSMPlayerCharacterFromActorInfo();
	if (!SourceCharacter)
	{
		NET_LOG(nullptr, Warning, TEXT("소스 캐릭터가 무효합니다."));
		EndAbilityByCancel();
		return;
	}

	USMAbilitySystemComponent* SourceASC = GetSMAbilitySystemComponentFromActorInfo();
	if (!ensureAlways(SourceASC))
	{
		EndAbilityByCancel();
		return;
	}

	LaunchData.SourceLocation = InStartLocation;
	LaunchData.SpawnRotationOffset.Empty();
	LaunchData.Count = 0;
	LaunchData.Rate = TotalLaunchTime / static_cast<float>(ProjectileCount - 1);
	LaunchData.bIsInverse = !LaunchData.bIsInverse;

	const float StartYawRotation = -ProjectileTotalAngle / 2.0f;
	const float SegmentYawRotation = ProjectileTotalAngle / (ProjectileCount - 1);
	const FRotator SourceRotation = InNormal.Rotation();
	for (int32 i = 0; i < ProjectileCount; ++i)
	{
		const float OffsetRotationYaw = i * SegmentYawRotation;
		const FRotator NewRotation(0.0, StartYawRotation + OffsetRotationYaw, 0.0);
		LaunchData.SpawnRotationOffset.Add(SourceRotation + NewRotation);
	}

	if (LaunchData.bIsInverse)
	{
		Algo::Reverse(LaunchData.SpawnRotationOffset);
	}

	// 첫발은 즉시 발사합니다.
	LaunchProjectileCallback();
}

void USMGameplayAbility_Launch_Piano::LaunchProjectileCallback()
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

	ASMProjectile* NewProjectile = CachedSMGameMode->GetProjectileFromProjectilePool(SourceTeam, ESMCharacterType::Piano); // 게임모드의 투사체풀을 통해 투사체를 가져옵니다.
	if (!NewProjectile)
	{
		EndAbilityByCancel();
		return;
	}

	const FVector LaunchLocation = bIsAttach ? SourceCharacter->GetActorLocation() : LaunchData.SourceLocation;
	NewProjectile->Launch(SourceCharacter, LaunchLocation, LaunchData.SpawnRotationOffset[LaunchData.Count++].Vector(), ProjectileSpeed, MaxDistance, Damage); // 투사체를 발사합니다.

	// FX를 실행합니다.
	FGameplayCueParameters GCParams;
	GCParams.TargetAttachComponent = SourceCharacter->GetRootComponent();
	GCParams.RawMagnitude = LaunchData.Count;
	SourceASC->ExecuteGameplayCue(SMTags::GameplayCue::ProjectileLaunch, GCParams);

	if (LaunchData.Count < ProjectileCount)
	{
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ThisClass::LaunchProjectileCallback, LaunchData.Rate);
	}
	else
	{
		K2_EndAbility();
	}
}