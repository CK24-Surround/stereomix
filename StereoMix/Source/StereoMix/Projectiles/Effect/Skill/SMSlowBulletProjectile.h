// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Projectiles/SMProjectile.h"
#include "SMSlowBulletProjectile.generated.h"

UCLASS(Abstract)
class STEREOMIX_API ASMSlowBulletProjectile : public ASMProjectile
{
	GENERATED_BODY()

public:
	ASMSlowBulletProjectile();

	/* 디버프 정보를 담습니다. 발사 전에 호출해야합니다. */
	void Init(float NewSlowDebuffMultiplier, float NewSlowDebuffDuration);

protected:
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	virtual void NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	bool IsValidateTarget(AActor* InTarget);

	void ApplyEffect(AActor* TargetActor);

	float SlowDebuffMultiplier = 0.0f;

	float SlowDebuffDuration = 0.0f;
};
