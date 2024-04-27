// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectiles/SMProjectilePool.h"

#include "SMProjectile.h"
#include "Utilities/SMLog.h"

USMProjectilePool::USMProjectilePool() {}

void USMProjectilePool::Init(const TSubclassOf<ASMProjectile>& InProjectileClassToSpawn)
{
	for (int32 i = 0; i < InitPoolSize; ++i)
	{
		// 투사체를 생성하고 생명주기의 끝에 회수되도록 델리게이트를 바인드합니다.
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		ASMProjectile* NewProjectile = GetWorld()->SpawnActor<ASMProjectile>(InProjectileClassToSpawn, Params);
		if (NewProjectile)
		{
			NewProjectile->OnEndLifeTime.BindUObject(this, &USMProjectilePool::ReturnProjectile);
			NewProjectile->SetFolderPath(TEXT("Projectile"));
			Pool.Add(NewProjectile);
		}
	}
}

ASMProjectile* USMProjectilePool::GetProjectile()
{
	if (!Pool.IsEmpty())
	{
		ASMProjectile* NewProjectile = Pool.Pop();
		return NewProjectile;
	}

	return nullptr;
}

void USMProjectilePool::ReturnProjectile(ASMProjectile* InProjectileToReturn)
{
	Pool.Push(InProjectileToReturn);
}

void USMProjectilePool::Expand() {}
