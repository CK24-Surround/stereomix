// Fill out your copyright notice in the Description page of Project Settings.


#include "StereoMixAbilityTask_SpawnAndLaunchProjectile.h"

#include "Projectiles/StereoMixProjectile.h"
#include "Utilities/StereoMixeLog.h"

UStereoMixAbilityTask_SpawnAndLaunchProjectile* UStereoMixAbilityTask_SpawnAndLaunchProjectile::CreateTask(UGameplayAbility* OwningAbility, TSubclassOf<AStereoMixProjectile> ProjectileClass, const FVector& StartLocation, const FRotator& StartRotation)
{
	UStereoMixAbilityTask_SpawnAndLaunchProjectile* NewTask = NewAbilityTask<UStereoMixAbilityTask_SpawnAndLaunchProjectile>(OwningAbility);
	if (NewTask)
	{
		NewTask->StartLocation = StartLocation;
		NewTask->StartRotation = StartRotation;
		NewTask->SpawnedProjectile = NewTask->BeginSpawningProjectile(ProjectileClass);
	}

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
	UWorld* World = Ability ? Ability->GetWorld() : nullptr;
	if (ensure(World))
	{
		AStereoMixProjectile* NewProjectile = World->SpawnActorDeferred<AStereoMixProjectile>(ProjectileClass, FTransform::Identity, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		if (ensure(NewProjectile))
		{
			AActor* AvatarActor = Ability->GetAvatarActorFromActorInfo();
			if (ensure(AvatarActor))
			{
				NewProjectile->SetOwner(AvatarActor);
			}

			return NewProjectile;
		}
	}

	return nullptr;
}

void UStereoMixAbilityTask_SpawnAndLaunchProjectile::FinishSpawningProjectile()
{
	if (SpawnedProjectile)
	{
		const FTransform NewTransform(StartRotation, StartLocation);
		SpawnedProjectile->FinishSpawning(NewTransform);
	}

	EndTask();
}
