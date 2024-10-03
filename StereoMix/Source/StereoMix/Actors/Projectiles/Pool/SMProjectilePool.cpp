// Copyright Surround, Inc. All Rights Reserved.


#include "SMProjectilePool.h"

#include "Actors/Projectiles/SMProjectile.h"


void USMProjectilePool::Init()
{
	if (!ensureAlways(ProjectileClass))
	{
		return;
	}

	// 기본 풀 사이즈 만큼 투사체를 생성합니다.
	for (int32 i = 0; i < InitPoolSize; ++i)
	{
		SpawnProjectile();
	}
}

ASMProjectile* USMProjectilePool::GetProjectile()
{
	ASMProjectile* NewProjectile = nullptr;

	// 풀이 비어 있다면 확장을 시도합니다.
	if (Pool.IsEmpty())
	{
		Expand(ExpandSize);
	}

	// 풀이 비어 있지 않다면 투사체를 꺼냅니다.
	if (!Pool.IsEmpty())
	{
		NewProjectile = Pool.Pop();
	}

	return NewProjectile;
}

void USMProjectilePool::ReturnProjectile(ASMProjectile* InProjectileToReturn)
{
	// OnEndLifeTime 델리게이트에 의해 호출되도록 설정해두었습니다.
	// 반환 요청된 투사체는 다시 풀에 넣습니다.
	Pool.Push(InProjectileToReturn);
}

void USMProjectilePool::Expand(int32 InExpandSize)
{
	// 확장 사이즈 만큼 투사체를 생성합니다.
	for (int32 i = 0; i < InExpandSize; ++i)
	{
		SpawnProjectile();
	}
}

void USMProjectilePool::SpawnProjectile()
{
	// 투사체를 생성하고 생명주기의 끝에 회수되도록 델리게이트를 바인드합니다.
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ASMProjectile* NewProjectile = GetWorld()->SpawnActor<ASMProjectile>(ProjectileClass, Params);
	if (ensure(NewProjectile))
	{
		NewProjectile->OnEndLifeTime.BindUObject(this, &USMProjectilePool::ReturnProjectile);
		Pool.Add(NewProjectile);
	}
}
