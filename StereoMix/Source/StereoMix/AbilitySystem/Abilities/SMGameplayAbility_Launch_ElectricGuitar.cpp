// Copyright Surround, Inc. All Rights Reserved.


#include "SMGameplayAbility_Launch_ElectricGuitar.h"

#include "Characters/SMPlayerCharacter.h"
#include "Games/SMGameMode.h"
#include "Projectiles/SMProjectile.h"
#include "Projectiles/SMProjectilePool.h"

class ASMGameMode;

void USMGameplayAbility_Launch_ElectricGuitar::LaunchProjectile(const FVector& InStartLocation, const FVector& InNormal)
{
	ASMPlayerCharacter* SourceCharacter = GetSMPlayerCharacterFromActorInfo();
	if (!ensureAlways(SourceCharacter))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	ASMGameMode* CachedSMGameMode = GetWorld()->GetAuthGameMode<ASMGameMode>();
	if (!ensureAlways(CachedSMGameMode))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	USMProjectilePool* ProjectilePool = CachedSMGameMode->GetEletricGuitarProjectilePool(SourceCharacter->GetTeam());
	if (!ensureAlways(ProjectilePool))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	ASMProjectile* NewProjectile = ProjectilePool->GetProjectile();
	if (!NewProjectile)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	// 투사체를 발사합니다.
	NewProjectile->Launch(SourceCharacter, InStartLocation, InNormal, ProjectileSpeed, MaxDistance, Damage);
}
