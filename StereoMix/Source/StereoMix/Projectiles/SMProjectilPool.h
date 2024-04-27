// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SMProjectilPool.generated.h"

class ASMProjectile;
/**
 * 
 */
UCLASS()
class STEREOMIX_API USMProjectilPool : public UObject
{
	GENERATED_BODY()

public:
	USMProjectilPool();

public:
	void Init(const TSubclassOf<ASMProjectile>& InProjectileClassToSpawn);

	ASMProjectile* GetProjectile();

	void ReturnProjectile(ASMProjectile* InProjectileToReturn);

	void Expand();

protected:
	UPROPERTY()
	TSubclassOf<ASMProjectile> ProjectileClassToSpawn;

	UPROPERTY()
	TArray<TObjectPtr<ASMProjectile>> Pool;

	UPROPERTY(EditAnywhere, Category = "Pool|Size")
	int32 InitPoolSize = 100;

	UPROPERTY(EditAnywhere, Category = "Pool|Size")
	int32 ExpandSize = 3;
};
