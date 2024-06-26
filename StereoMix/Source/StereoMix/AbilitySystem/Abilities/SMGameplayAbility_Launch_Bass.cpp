// Copyright Surround, Inc. All Rights Reserved.


#include "SMGameplayAbility_Launch_Bass.h"

#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "Characters/SMPlayerCharacter.h"
#include "Games/SMGameMode.h"
#include "Projectiles/SMProjectile.h"

void USMGameplayAbility_Launch_Bass::LaunchProjectile(const FVector& InStartLocation, const FVector& InNormal)
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

	ASMProjectile* NewProjectile = CachedSMGameMode->GetProjectileFromProjectilePool(SourceTeam, ESMCharacterType::Bass); // 게임모드의 투사체풀을 통해 투사체를 가져옵니다.
	if (!NewProjectile)
	{
		EndAbilityByCancel();
		return;
	}

	NewProjectile->Launch(SourceCharacter, InStartLocation, InNormal, ProjectileSpeed, MaxDistance, Damage); // 투사체를 발사합니다.

	// FX를 실행합니다.
	FGameplayCueParameters GCParams;
	GCParams.TargetAttachComponent = SourceCharacter->GetRootComponent();
	GCParams.RawMagnitude = 1;
	SourceASC->ExecuteGameplayCue(SMTags::GameplayCue::ProjectileLaunch, GCParams);

	K2_EndAbility();
}
