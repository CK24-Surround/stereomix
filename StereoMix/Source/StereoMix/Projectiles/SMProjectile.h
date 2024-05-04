// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "Utilities/SMTeam.h"
#include "SMProjectile.generated.h"

DECLARE_DELEGATE_OneParam(FOnProjectileLifeTimeSignature, ASMProjectile* /*Projectile*/);

class UNiagaraComponent;
class UNiagaraSystem;
class USMProjectileAssetData;
class USMTeamComponent;
class UGameplayEffect;
class USMDesignData;
class USphereComponent;
class UProjectileMovementComponent;

UCLASS()
class STEREOMIX_API ASMProjectile : public AActor
{
	GENERATED_BODY()

public:
	ASMProjectile();

public:
	virtual void PostInitializeComponents() override;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

public:
	/** 투사체를 발사합니다. 서버에서 호출되야합니다. */
	void Launch(AActor* NewOwner, const FVector_NetQuantize10& InStartLocation, const FVector_NetQuantizeNormal& InNormal, float InSpeed, float InMaxDistance, float InDamage);

protected:
	/** 서버, 클라이언트에서 모두 발사되도록 합니다. Launch를 통해 호출됩니다.*/
	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastRPCLaunch(const FVector_NetQuantize10& InStartLocation, const FVector_NetQuantizeNormal& InNormal, float InSpeed, float InMaxDistance);

	/** 투사체가 활동을 시작할때 꼭 호출해줘야합니다. 투사체 활성화와 관련된 처리를 담당합니다. */
	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastRPCStartLifeTime();

	/**
	 * 투사체가 풀로 반환되야할때 꼭 호출해줘야합니다. 투사체 비활성화와 관련된 처리를 수행하고 델리게이트를 브로드캐스트해 자동으로 풀로 돌아갑니다.
	 * Destroy를 대체한다고 생각하면 편합니다. 
	 */
	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastRPCEndLifeTime();

protected:
	/** 투사체가 최대 사정거리를 벗어나는 경우 투사체를 풀로 반환합니다. 사정거리 구현을 위해 사용됩니다. */
	void ReturnToPoolIfOutOfMaxDistance();

// ~Component Section
protected:
	UFUNCTION()
	void OnChangeTeamCallback();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Collision")
	TObjectPtr<USphereComponent> SphereComponent;

	UPROPERTY(VisibleAnywhere, Category = "FX")
	TObjectPtr<UNiagaraComponent> ProjectileFXComponent;

	UPROPERTY(VisibleAnywhere, Category = "Projectile")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent;

	UPROPERTY(VisibleAnywhere, Category = "Team")
	TObjectPtr<USMTeamComponent> TeamComponent;
// ~Component Section

// ~FX Section
protected:
	UPROPERTY(EditAnywhere, Category = "FX")
	TObjectPtr<UNiagaraSystem> ProjectileBodyFX;
// ~FX Section

public:
	/** 생명 주기가 끝나 회수되야할때 브로드캐스트합니다. */
	FOnProjectileLifeTimeSignature OnEndLifeTime;

protected:
	FVector StartLocation;

	float MaxDistance;

	float Damage;

protected:
	UPROPERTY(EditAnywhere, Category = "GAS|Tags")
	FGameplayTagContainer IgnoreTargetStateTags;

	UPROPERTY(EditAnywhere, Category = "GAS|GE")
	TSubclassOf<UGameplayEffect> DamageGE;
};
