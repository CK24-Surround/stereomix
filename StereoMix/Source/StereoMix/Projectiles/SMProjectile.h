// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Data/SMTeam.h"
#include "GameplayTagContainer.h"
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

UCLASS()
class STEREOMIX_API ASMProjectile : public AActor, public ISMTeamInterface
{
	GENERATED_BODY()

public:
	ASMProjectile();

	/** 투사체를 발사합니다. 서버에서 호출되어야합니다. */
	void Launch(AActor* NewOwner, const FVector_NetQuantize10& InStartLocation, const FVector_NetQuantizeNormal& InNormal, float InSpeed, float InMaxDistance, float InMagnitude = 0.0f);

	/** 투사체가 활동을 시작할때 꼭 호출해줘야합니다. 투사체 활성화와 관련된 처리를 담당합니다. 서버에서 호출되어야합니다.*/
	void StartLifeTime();

	/**
	 * 투사체가 풀로 반환되야할때 꼭 호출해줘야합니다. 투사체 비활성화와 관련된 처리를 수행하고 델리게이트를 브로드캐스트해 자동으로 풀로 돌아갑니다. 서버에서 호출되어야합니다.
	 * 사실상 Destroy를 대체합니다.
	 */
	void EndLifeTime();

	FORCEINLINE virtual USMTeamComponent* GetTeamComponent() const override { return TeamComponent; }

	virtual ESMTeam GetTeam() const override;

	/** 생명 주기가 끝나 회수되야할때 투사체 풀에 알리기 위한 델리게이트입니다. */
	FOnProjectileLifeTimeSignature OnEndLifeTime;

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

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
	virtual bool IsValidateTarget(AActor* InTarget);

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

	float Magnitude;

	UPROPERTY(EditAnywhere, Category = "GAS|Tags")
	FGameplayTagContainer IgnoreTargetStateTags;

	UPROPERTY(EditAnywhere, Category = "GAS|GE")
	TSubclassOf<UGameplayEffect> DamageGE;
};
