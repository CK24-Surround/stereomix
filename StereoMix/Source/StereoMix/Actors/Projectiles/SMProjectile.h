// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Data/SMTeam.h"
#include "Data/SMProjectileParameters.h"
#include "Interfaces/SMTeamInterface.h"

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

UCLASS(Abstract)
class STEREOMIX_API ASMProjectile : public AActor, public ISMTeamInterface
{
	GENERATED_BODY()

public:
	ASMProjectile();

	/** 투사체를 발사합니다. 서버에서 호출되어야합니다. */
	void Launch(const FSMProjectileParameters& InParameters);

	FORCEINLINE virtual USMTeamComponent* GetTeamComponent() const override { return TeamComponent; }

	virtual ESMTeam GetTeam() const override;

	/** 생명 주기가 끝나 회수되야할때 투사체 풀에 알리기 위한 델리게이트입니다. */
	FOnProjectileLifeTimeSignature OnEndLifeTime;

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	/** 투사체가 발사되기 전에 추가로 설정해야할 값이 있다면 이 함수를 오버라이딩해 구현하면됩니다. */
	virtual void PreLaunch(const FSMProjectileParameters& InParameters) {}

	/** 투사체가 활동을 시작할때 꼭 호출해줘야합니다. 투사체 활성화와 관련된 처리를 담당합니다. 서버에서 호출되어야합니다.*/
	void StartLifeTime();

	/**
	 * 투사체가 풀로 반환되야할때 꼭 호출해줘야합니다. 투사체 비활성화와 관련된 처리를 수행하고 델리게이트를 브로드캐스트해 자동으로 풀로 돌아갑니다. 서버에서 호출되어야합니다.
	 * 사실상 Destroy를 대체합니다.
	 */
	void EndLifeTime();

	/** 실제로 투사체를 발사합니다. Launch를 통해 호출됩니다.*/
	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastLaunchInternal(const FVector_NetQuantize10& InStartLocation, const FVector_NetQuantizeNormal& InNormal, float InSpeed, float InMaxDistance);

	/** 활성화하는데 필요한 처리를 합니다. StartLifeTime을 통해 호출됩니다. */
	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastStartLifeTimeInternal();

	/** 비활성화하는데 필요한 처리를 합니다. EndLifeTime을 통해 호출됩니다. */
	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastEndLifeTimeInternal();

	/** 투사체가 최대 사정거리를 벗어나는 경우 투사체를 풀로 반환합니다. 사정거리 구현을 위해 사용됩니다. */
	void ReturnToPoolIfOutOfMaxDistance();

	/** 타겟이 유효한지 확인합니다. 타겟의 유효성을 검증해야할때 사용합니다. 서버에서만 사용되어야합니다. */
	virtual bool IsValidTarget(AActor* InTarget);

	/** 투사체에 이펙트를 적용하려면 이 함수를 오버라이딩해 구현하면됩니다. 서버에서 호출됩니다. */
	virtual void AddProjectileFX() {}

	/** 투사체가 풀에 돌아가기 전에 이펙트를 제거하려면 이 함수를 오버라이딩해 구현하면됩니다. 서버에서 호출됩니다. */
	virtual void RemoveProjectileFX() {}

	UPROPERTY(VisibleAnywhere, Category = "Collision")
	TObjectPtr<USphereComponent> SphereComponent;

	UPROPERTY(VisibleAnywhere, Category = "FX")
	TObjectPtr<UNiagaraComponent> ProjectileBodyFXComponent;

	UPROPERTY(VisibleAnywhere, Category = "Projectile")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent;

	UPROPERTY(VisibleAnywhere, Category = "Team")
	TObjectPtr<USMTeamComponent> TeamComponent;

	FVector StartLocation;

	float MaxDistance;
};
