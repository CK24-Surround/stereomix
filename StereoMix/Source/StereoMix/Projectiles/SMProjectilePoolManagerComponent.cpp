// Copyright Surround, Inc. All Rights Reserved.


#include "SMProjectilePoolManagerComponent.h"

#include "SMProjectilePool.h"
#include "Utilities/SMLog.h"


USMProjectilePoolManagerComponent::USMProjectilePoolManagerComponent()
{
	bWantsInitializeComponent = true;
	SetIsReplicatedByDefault(true);
}

void USMProjectilePoolManagerComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void USMProjectilePoolManagerComponent::InitializeComponent()
{
	Super::InitializeComponent();

	if (GetOwnerRole() == ROLE_Authority)
	{
		InitPool(PianoProjectilePoolClass, PianoProjectilePool);
		InitPool(ElectricGuitarProjectilePoolClass, ElectricGuitarProjectilePool);
		InitPool(SlowBulletPoolClass, SlowBulletPool);
	}
}

void USMProjectilePoolManagerComponent::InitPool(const TMap<ESMTeam, TSubclassOf<USMProjectilePool>>& ProjectilePoolClass, TMap<ESMTeam, TObjectPtr<USMProjectilePool>>& ProjectilePool)
{
	for (const auto& [Team, PoolClass] : ProjectilePoolClass)
	{
		if (!PoolClass)
		{
			continue;
		}

		USMProjectilePool* NewObj = NewObject<USMProjectilePool>(this, PoolClass);
		NewObj->Init();
		ProjectilePool.FindOrAdd(Team) = NewObj;
		NET_LOG(GetOwner(), Warning, TEXT("생성된 투사체풀: %p"), *GetNameSafe(NewObj));
	}
}

ASMProjectile* USMProjectilePoolManagerComponent::GetProjectile(ESMTeam SourceTeam, const TMap<ESMTeam, TObjectPtr<USMProjectilePool>>& ProjectilePool) const
{
	const TObjectPtr<USMProjectilePool>* PoolPtr = ProjectilePool.Find(SourceTeam);
	if (!PoolPtr)
	{
		return nullptr;
	}

	USMProjectilePool* Pool = *PoolPtr;
	if (!Pool)
	{
		return nullptr;
	}

	return Pool->GetProjectile();
}
