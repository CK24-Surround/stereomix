// Copyright Studio Surround. All Rights Reserved.


#include "SMProjectileFunctionLibrary.h"

#include "GameFramework/GameStateBase.h"
#include "Components/Core/SMProjectilePoolManagerComponent.h"


USMProjectilePoolManagerComponent* USMProjectileFunctionLibrary::GetPoolManager(const UWorld* World)
{
	const AGameStateBase* GameState = World ? World->GetGameState() : nullptr;
	return GameState ? GameState->GetComponentByClass<USMProjectilePoolManagerComponent>() : nullptr;
}

ASMProjectile* USMProjectileFunctionLibrary::GetElectricGuitarProjectile(const UWorld* World, ESMTeam Team)
{
	const USMProjectilePoolManagerComponent* PoolManager = GetPoolManager(World);
	return PoolManager ? PoolManager->GetElectricGuitarProjectile(Team) : nullptr;
}

ASMProjectile* USMProjectileFunctionLibrary::GetPianoProjectile(const UWorld* World, ESMTeam Team, int32 ChargeLevel)
{
	const USMProjectilePoolManagerComponent* PoolManager = GetPoolManager(World);
	return PoolManager ? PoolManager->GetPianoProjectile(Team, ChargeLevel) : nullptr;
}

ASMProjectile* USMProjectileFunctionLibrary::GetSlowBulletProjectile(const UWorld* World, ESMTeam Team)
{
	const USMProjectilePoolManagerComponent* PoolManager = GetPoolManager(World);
	return PoolManager ? PoolManager->GetSlowBulletProjectile(Team) : nullptr;
}
