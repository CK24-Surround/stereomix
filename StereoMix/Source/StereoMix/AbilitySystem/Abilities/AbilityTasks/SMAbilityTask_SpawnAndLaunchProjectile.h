// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "SMAbilityTask_SpawnAndLaunchProjectile.generated.h"

class ASMProjectile;
/**
 * 
 */
UCLASS()
class STEREOMIX_API USMAbilityTask_SpawnAndLaunchProjectile : public UAbilityTask
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", DisplayName = "Spawn Projectile", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"))
	static USMAbilityTask_SpawnAndLaunchProjectile* CreateTask(UGameplayAbility* OwningAbility, TSubclassOf<ASMProjectile> ProjectileClass, const FVector& StartLocation, const FRotator& StartRotation);

protected:
	virtual void Activate() override;

	virtual void OnDestroy(bool bInOwnerFinished) override;

protected:
	ASMProjectile* BeginSpawningProjectile(TSubclassOf<ASMProjectile> ProjectileClass);
	void FinishSpawningProjectile();

protected:
	UPROPERTY()
	TObjectPtr<ASMProjectile> SpawnedProjectile;

	FVector StartLocation;
	FRotator StartRotation;
};
