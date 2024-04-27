// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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
	/** 풀을 채우고 사용할 준비를 합니다. 투사체 풀을 사용하기전에 꼭 호출해줘야합니다. */
	void Init(const TSubclassOf<ASMProjectile>& InProjectileClassToSpawn);

	/** 투사체를 풀에서 꺼냅니다. */
	ASMProjectile* GetProjectile();

	/** 투사체를 풀로 반환합니다. 수동으로 반환할수도 있고 델리게이트를 통해서도 반환할 수 있습니다. */
	void ReturnProjectile(ASMProjectile* InProjectileToReturn);

	/** 투사체 풀의 크기를 확장합니다. 투사체 수가 부족한 경우 자동으로 확장합니다. 수동으로 호출할수도 있습니다. */
	void Expand(int32 InExpandSize);

protected:
	/** 투사체를 생성하고 EndLifeTime 델리게이트에 바인드합니다. */
	void SpawnProjectile();

protected:
	UPROPERTY()
	TSubclassOf<ASMProjectile> ProjectileClassToSpawn;

	UPROPERTY()
	TArray<TObjectPtr<ASMProjectile>> Pool;

	UPROPERTY(EditAnywhere, Category = "Pool|Size", DisplayName = "기본 풀 사이즈")
	int32 InitPoolSize = 30;

	UPROPERTY(EditAnywhere, Category = "Pool|Size", DisplayName = "확장 시 추가할 풀 사이즈")
	int32 ExpandSize = 5;
};
