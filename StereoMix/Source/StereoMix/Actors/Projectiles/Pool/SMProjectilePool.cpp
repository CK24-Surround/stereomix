// Copyright Surround, Inc. All Rights Reserved.


#include "SMProjectilePool.h"

#include "Actors/Projectiles/SMProjectile.h"


void USMProjectilePool::Init(const TSubclassOf<ASMProjectile>& ProjectileClassToSpawn, int32 DefaultSize, int32 DefaultExpansionSize)
{
	if (!ProjectileClassToSpawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("유효하지 않은 투사체 클래스입니다. 투사체풀 초기화에 실패했습니다. "));
		return;
	}

	ProjectileClass = ProjectileClassToSpawn;
	ExpandSize = DefaultExpansionSize;

	for (int32 i = 0; i < DefaultSize; ++i) // 기본 풀 사이즈 만큼 투사체를 생성합니다.
	{
		SpawnAndBindProjectile();
	}
}

ASMProjectile* USMProjectilePool::GetProjectile()
{
	ASMProjectile* NewProjectile = nullptr;

	if (Pool.IsEmpty()) // 풀이 비어 있다면 확장을 시도합니다.
	{
		Expand(ExpandSize);
	}
	else // 풀이 비어 있지 않다면 투사체를 꺼냅니다.
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
		SpawnAndBindProjectile();
	}
}

void USMProjectilePool::SpawnAndBindProjectile()
{
	// 투사체를 생성하고 생명주기의 끝에 회수되도록 델리게이트를 바인드합니다.
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	UWorld* World = GetWorld();
	if (ASMProjectile* NewProjectile = World ? World->SpawnActor<ASMProjectile>(ProjectileClass, Params) : nullptr)
	{
		NewProjectile->OnEndLifeTime.BindUObject(this, &USMProjectilePool::ReturnProjectile);
		Pool.Push(NewProjectile);
	}
}
