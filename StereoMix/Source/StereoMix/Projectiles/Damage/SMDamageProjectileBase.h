// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Projectiles/SMProjectile.h"

#include "SMDamageProjectileBase.generated.h"

UCLASS(Abstract)
class STEREOMIX_API ASMDamageProjectileBase : public ASMProjectile
{
	GENERATED_BODY()

public:
	ASMDamageProjectileBase();

protected:
	virtual bool IsValidateTarget(AActor* InTarget) override;

	/** 오버렙 판정 발생 시 호출됩니다. 보통 플레이어와 충돌할때 호출됩니다.*/
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	void HandleHit(AActor* InTarget);

	/** GE를 통해 데미지를 적용합니다. */
	void ApplyDamage(AActor* InTarget);

	/** 어떤 이펙트를 재생할지 하위 클래스에서 직접 구현해줘야합니다. 서버에서 호출됩니다. */
	virtual void PlayHitFX(AActor* InTarget) {}

	/** 블로킹 판정 발생 시 호출됩니다. 보통 벽에 부딪힐때 호출됩니다. */
	virtual void NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	void HandleWallHit(const FVector& HitLocation);

	/** 벽 충돌시 재생할 이펙트를 하위 클래스에서 직접 구현해줘야합니다. 서버에서 호출됩니다. */
	virtual void PlayWallHitFX(const FVector& HitLocation) {}
};
