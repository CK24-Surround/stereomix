// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Projectiles/SMProjectile.h"
#include "SMEffectProjectileBase.generated.h"

UCLASS(Abstract)
class STEREOMIX_API ASMEffectProjectileBase : public ASMProjectile
{
	GENERATED_BODY()

protected:
	virtual void PreLaunch(const FSMProjectileParameters& InParameters) override;

	virtual bool IsValidTarget(AActor* InTarget) override;

	/** 오버렙 판정 발생 시 호출됩니다. 보통 플레이어와 충돌할때 호출됩니다.*/
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	/* 대상에 적중했을때 처리할 효과들을 추가하고 싶다면 이 함수를 오버라이딩하면 됩니다. */
	virtual void HandleHitEffect(AActor* InTarget);

	/** 어떤 이펙트를 재생할지 하위 클래스에서 직접 구현해줘야합니다. 서버에서 호출됩니다. */
	virtual void PlayHitFX(AActor* InTarget) {}

	/** 블로킹 판정 발생 시 호출됩니다. 보통 벽에 부딪힐때 호출됩니다. */
	virtual void NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	/* 벽에 적중했을때 처리할 효과들을 추가하고 싶다면 이 함수를 오버라이딩하면 됩니다. */
	virtual void HandleWallHitEffect(const FVector& HitLocation);

	/** 벽 충돌시 재생할 이펙트를 하위 클래스에서 직접 구현해줘야합니다. 서버에서 호출됩니다. */
	virtual void PlayWallHitFX(const FVector& HitLocation) {}

	/** GE를 통해 데미지를 적용합니다. 데미지를 주는 기능이 필요하다면 사용하면 됩니다. */
	void ApplyDamage(AActor* InTarget);

	float Damage = 0.0f;
};
