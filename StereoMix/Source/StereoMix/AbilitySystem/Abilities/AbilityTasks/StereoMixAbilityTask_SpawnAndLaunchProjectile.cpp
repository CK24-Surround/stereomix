// Fill out your copyright notice in the Description page of Project Settings.


#include "StereoMixAbilityTask_SpawnAndLaunchProjectile.h"

#include "Projectiles/SMProjectile.h"

UStereoMixAbilityTask_SpawnAndLaunchProjectile* UStereoMixAbilityTask_SpawnAndLaunchProjectile::CreateTask(UGameplayAbility* OwningAbility, TSubclassOf<ASMProjectile> ProjectileClass, const FVector& StartLocation, const FRotator& StartRotation)
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

ASMProjectile* UStereoMixAbilityTask_SpawnAndLaunchProjectile::BeginSpawningProjectile(TSubclassOf<ASMProjectile> ProjectileClass)
{
	UWorld* World = Ability ? Ability->GetWorld() : nullptr;
	if (ensure(World))
	{
		ASMProjectile* NewProjectile = World->SpawnActorDeferred<ASMProjectile>(ProjectileClass, FTransform::Identity, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
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
