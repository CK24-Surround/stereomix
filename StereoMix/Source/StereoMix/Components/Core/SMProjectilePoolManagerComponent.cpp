// Copyright Surround, Inc. All Rights Reserved.


#include "SMProjectilePoolManagerComponent.h"

#include "Actors/Projectiles/SMProjectile.h"
#include "Actors/Projectiles/Pool/SMProjectilePool.h"
#include "Utilities/SMLog.h"


USMProjectilePoolManagerComponent::USMProjectilePoolManagerComponent()
{
	bWantsInitializeComponent = true;
	SetIsReplicatedByDefault(true);
}

void USMProjectilePoolManagerComponent::InitializeComponent()
{
	Super::InitializeComponent();

	if (GetOwnerRole() == ROLE_Authority)
	{
		auto CreateAndInitPool = [&](const FSMProjectileSpawnData& ProjectileSpawnData, TMap<ESMTeam, TObjectPtr<USMProjectilePool>>& ProjectilePool) {
			const auto& [TeamProjectileClassMapToSpawn, DefaultPoolSize, DefaultPoolExpansionSize] = ProjectileSpawnData;
			for (const auto& [Team, ProjectileClassToSpawn] : TeamProjectileClassMapToSpawn)
			{
				if (USMProjectilePool* NewPool = NewObject<USMProjectilePool>(this))
				{
					NewPool->Init(ProjectileClassToSpawn, DefaultPoolSize, DefaultPoolExpansionSize);
					ProjectilePool.FindOrAdd(Team) = NewPool;
					NET_LOG(GetOwner(), Log, TEXT("%s로 투사체풀 %s(이)가 생성되었습니다."), *GetNameSafe(ProjectileClassToSpawn), *GetNameSafe(NewPool));
				}
			}
		};

		CreateAndInitPool(PianoProjectile1SpawnData, PianoProjectile1Pool);
		CreateAndInitPool(PianoProjectile2SpawnData, PianoProjectile2Pool);
		CreateAndInitPool(ElectricGuitarProjectileSpawnData, ElectricGuitarProjectilePool);
		CreateAndInitPool(SlowBulletSpawnData, SlowBulletProjectilePool);
	}
}

ASMProjectile* USMProjectilePoolManagerComponent::GetElectricGuitarProjectile(ESMTeam SourceTeam) const
{
	return GetProjectile(SourceTeam, ElectricGuitarProjectilePool);
}

ASMProjectile* USMProjectilePoolManagerComponent::GetPianoProjectile(ESMTeam SourceTeam, int32 ChargeLevel) const
{
	if (ChargeLevel == 1)
	{
		return GetProjectile(SourceTeam, PianoProjectile1Pool);
	}

	if (ChargeLevel == 2)
	{
		return GetProjectile(SourceTeam, PianoProjectile2Pool);
	}

	return nullptr;
}

ASMProjectile* USMProjectilePoolManagerComponent::GetSlowBulletProjectile(ESMTeam SourceTeam) const
{
	return GetProjectile(SourceTeam, SlowBulletProjectilePool);
}

ASMProjectile* USMProjectilePoolManagerComponent::GetProjectile(ESMTeam ProjectileTeam, const TMap<ESMTeam, TObjectPtr<USMProjectilePool>>& ProjectilePool)
{
	const TObjectPtr<USMProjectilePool>* PoolPtr = ProjectilePool.Find(ProjectileTeam);
	if (USMProjectilePool* Pool = PoolPtr ? *PoolPtr : nullptr)
	{
		return Pool->GetProjectile();
	}

	return nullptr;
}
