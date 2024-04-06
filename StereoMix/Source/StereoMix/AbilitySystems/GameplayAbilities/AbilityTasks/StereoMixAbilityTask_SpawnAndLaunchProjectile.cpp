// Fill out your copyright notice in the Description page of Project Settings.


#include "StereoMixAbilityTask_SpawnAndLaunchProjectile.h"

#include "Projectiles/StereoMixProjectile.h"
#include "Utilities/StereoMixeLog.h"

UStereoMixAbilityTask_SpawnAndLaunchProjectile* UStereoMixAbilityTask_SpawnAndLaunchProjectile::CreateTask(UGameplayAbility* OwningAbility, TSubclassOf<AStereoMixProjectile> ProjectileClass, const FVector& StartLocation, const FVector& StartDirection)
{
	if (!OwningAbility)
	{
		NET_LOG(nullptr, Error, TEXT("Task 생성에 실패했습니다: OwningAbility가 유효하지 않습니다."));
		return nullptr;
	}

	UStereoMixAbilityTask_SpawnAndLaunchProjectile* NewTask = NewAbilityTask<UStereoMixAbilityTask_SpawnAndLaunchProjectile>(OwningAbility);
	if (!NewTask)
	{
		NET_LOG(nullptr, Error, TEXT("Task 생성에 실패했습니다."));
		return nullptr;
	}

	NewTask->StartLocation = StartLocation;
	NewTask->StartDirection = StartDirection;
	NewTask->SpawnedProjectile = NewTask->BeginSpawningProjectile(ProjectileClass);

	return NewTask;
}

void UStereoMixAbilityTask_SpawnAndLaunchProjectile::Activate()
{
	FinishSpawningProjectile();
}

void UStereoMixAbilityTask_SpawnAndLaunchProjectile::OnDestroy(bool bInOwnerFinished)
{
	Super::OnDestroy(bInOwnerFinished);
}

AStereoMixProjectile* UStereoMixAbilityTask_SpawnAndLaunchProjectile::BeginSpawningProjectile(TSubclassOf<AStereoMixProjectile> ProjectileClass)
{
	if (!Ability)
	{
		NET_LOG(nullptr, Error, TEXT("투사체 생성에 실패했습니다: Ability가 유효하지 않습니다."));
		return nullptr;
	}

	const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
	if (!ActorInfo)
	{
		NET_LOG(nullptr, Error, TEXT("투사체 생성에 실패했습니다: ActorInfo가 유효하지않습니다."));
		return nullptr;
	}

	UWorld* World = Ability->GetWorld();
	if (!World)
	{
		NET_LOG(ActorInfo->AvatarActor.Get(), Error, TEXT("투사체 생성에 실패했습니다: 월드가 유효하지않습니다."));
		return nullptr;
	}

	if (!ActorInfo->IsNetAuthority())
	{
		NET_LOG(ActorInfo->AvatarActor.Get(), Warning, TEXT("투사체 생성이 클라이언트에서 시도되었습니다. 투사체는 클라이언트에서 생성되서는 안 됩니다."));
		return nullptr;
	}

	AStereoMixProjectile* NewProjectile = World->SpawnActorDeferred<AStereoMixProjectile>(ProjectileClass, FTransform::Identity, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (!NewProjectile)
	{
		NET_LOG(ActorInfo->AvatarActor.Get(), Error, TEXT("투사체 생성에 실패했습니다."));
		return nullptr;
	}

	NewProjectile->SetOwner(ActorInfo->AvatarActor.Get());

	return NewProjectile;
}

void UStereoMixAbilityTask_SpawnAndLaunchProjectile::FinishSpawningProjectile()
{
	if (SpawnedProjectile)
	{
		const FRotator NewRotator = FRotationMatrix::MakeFromX(StartDirection).Rotator();
		const FTransform NewTransform(NewRotator, StartLocation);
		SpawnedProjectile->FinishSpawning(NewTransform);
	}

	EndTask();
}
