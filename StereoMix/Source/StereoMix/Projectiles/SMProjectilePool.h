// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SMProjectilePool.generated.h"

class ASMProjectile;
/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class STEREOMIX_API USMProjectilePool : public UObject
{
	GENERATED_BODY()

public:
	USMProjectilePool();

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
	int32 InitPoolSize = 3;

	UPROPERTY(EditAnywhere, Category = "Pool|Size")
	int32 ExpandSize = 3;
};
