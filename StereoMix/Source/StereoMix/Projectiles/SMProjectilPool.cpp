// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectiles/SMProjectilPool.h"

#include "SMProjectile.h"

USMProjectilPool::USMProjectilPool() {}

void USMProjectilPool::Init(const TSubclassOf<ASMProjectile>& InProjectileClassToSpawn)
{
	for (int32 i = 0; i < InitPoolSize; ++i)
	{
		ASMProjectile* NewProjectile = GetWorld()->SpawnActor<ASMProjectile>(InProjectileClassToSpawn);
		Pool.Add(NewProjectile);
	}
}

ASMProjectile* USMProjectilPool::GetProjectile()
{
	return nullptr;
}

void USMProjectilPool::ReturnProjectile(ASMProjectile* InProjectileToReturn) {}

void USMProjectilPool::Expand() {}
