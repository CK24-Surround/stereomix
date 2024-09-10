// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/SMTeam.h"
#include "SMProjectilePoolManagerComponent.generated.h"


class USMProjectilePool;
class ASMProjectile;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STEREOMIX_API USMProjectilePoolManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	USMProjectilePoolManagerComponent();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void InitializeComponent() override;

	/** 서버에서만 수행 가능합니다. */
	ASMProjectile* GetProjectileForPiano(ESMTeam SourceTeam) const { return GetProjectile(SourceTeam, PianoProjectilePool); }

	/** 서버에서만 수행 가능합니다. */
	ASMProjectile* GetProjectileForElectricGuitar(ESMTeam SourceTeam) const { return GetProjectile(SourceTeam, ElectricGuitarProjectilePool); }

protected:
	void InitPool(const TMap<ESMTeam, TSubclassOf<USMProjectilePool>>& ProjectilePoolClass, TMap<ESMTeam, TObjectPtr<USMProjectilePool>>& ProjectilePool);

	ASMProjectile* GetProjectile(ESMTeam SourceTeam, const TMap<ESMTeam, TObjectPtr<USMProjectilePool>>& ProjectilePool) const;

	UPROPERTY(EditAnywhere, Category = "Design")
	TMap<ESMTeam, TSubclassOf<USMProjectilePool>> PianoProjectilePoolClass;

	UPROPERTY()
	TMap<ESMTeam, TObjectPtr<USMProjectilePool>> PianoProjectilePool;

	UPROPERTY(EditAnywhere, Category = "Design")
	TMap<ESMTeam, TSubclassOf<USMProjectilePool>> ElectricGuitarProjectilePoolClass;

	UPROPERTY()
	TMap<ESMTeam, TObjectPtr<USMProjectilePool>> ElectricGuitarProjectilePool;
};
