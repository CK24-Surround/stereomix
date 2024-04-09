// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "StereoMixAbilityTask_SpawnAndLaunchProjectile.generated.h"

class AStereoMixProjectile;
/**
 * 
 */
UCLASS()
class STEREOMIX_API UStereoMixAbilityTask_SpawnAndLaunchProjectile : public UAbilityTask
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", DisplayName = "Spawn Projectile", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"))
	static UStereoMixAbilityTask_SpawnAndLaunchProjectile* CreateTask(UGameplayAbility* OwningAbility, TSubclassOf<AStereoMixProjectile> ProjectileClass, const FVector& StartLocation, const FRotator& StartRotation);

protected:
	virtual void Activate() override;

	virtual void OnDestroy(bool bInOwnerFinished) override;

protected:
	AStereoMixProjectile* BeginSpawningProjectile(TSubclassOf<AStereoMixProjectile> ProjectileClass);
	void FinishSpawningProjectile();

protected:
	UPROPERTY()
	TObjectPtr<AStereoMixProjectile> SpawnedProjectile;

	FVector StartLocation;
	FRotator StartRotation;
};
