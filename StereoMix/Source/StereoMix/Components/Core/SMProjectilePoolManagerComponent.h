// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/SMTeam.h"
#include "SMProjectilePoolManagerComponent.generated.h"

class USMProjectilePool;
class ASMProjectile;

USTRUCT(BlueprintType)
struct FSMProjectileSpawnData
{
	GENERATED_BODY()

	FSMProjectileSpawnData()
	{
		for (int32 i = 1; i < static_cast<int32>(ESMTeam::Max); ++i)
		{
			ESMTeam Team = static_cast<ESMTeam>(i);
			TeamProjectileClassMapToSpawn.Add(Team, nullptr);
		}
	}

	UPROPERTY(EditAnywhere)
	TMap<ESMTeam, TSubclassOf<ASMProjectile>> TeamProjectileClassMapToSpawn;

	UPROPERTY(EditAnywhere, meta = (ClampMin = 1))
	int32 DefaultPoolSize = 10;

	UPROPERTY(EditAnywhere, meta = (ClampMin = 1))
	int32 DefaultPoolExpansionSize = 5;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STEREOMIX_API USMProjectilePoolManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	USMProjectilePoolManagerComponent();

	virtual void InitializeComponent() override;

	/** 서버에서만 수행 가능합니다. */
	ASMProjectile* GetElectricGuitarProjectile(ESMTeam SourceTeam) const;

	/** 서버에서만 수행 가능합니다. */
	ASMProjectile* GetPianoProjectile(ESMTeam SourceTeam, int32 ChargeLevel) const;

	/** 서버에서만 수행 가능합니다. */
	ASMProjectile* GetSlowBulletProjectile(ESMTeam SourceTeam) const;

protected:
	static ASMProjectile* GetProjectile(ESMTeam ProjectileTeam, const TMap<ESMTeam, TObjectPtr<USMProjectilePool>>& ProjectilePool);

	UPROPERTY(EditAnywhere, Category = "Design")
	FSMProjectileSpawnData ElectricGuitarProjectileSpawnData;

	UPROPERTY()
	TMap<ESMTeam, TObjectPtr<USMProjectilePool>> ElectricGuitarProjectilePool;

	UPROPERTY(EditAnywhere, Category = "Design")
	FSMProjectileSpawnData PianoProjectile1SpawnData;

	UPROPERTY()
	TMap<ESMTeam, TObjectPtr<USMProjectilePool>> PianoProjectile1Pool;

	UPROPERTY(EditAnywhere, Category = "Design")
	FSMProjectileSpawnData PianoProjectile2SpawnData;

	UPROPERTY()
	TMap<ESMTeam, TObjectPtr<USMProjectilePool>> PianoProjectile2Pool;

	UPROPERTY(EditAnywhere, Category = "Design")
	FSMProjectileSpawnData SlowBulletSpawnData;

	UPROPERTY()
	TMap<ESMTeam, TObjectPtr<USMProjectilePool>> SlowBulletProjectilePool;
};
