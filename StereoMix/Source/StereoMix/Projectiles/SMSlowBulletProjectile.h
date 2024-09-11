// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMProjectile.h"
#include "SMSlowBulletProjectile.generated.h"

UCLASS()
class STEREOMIX_API ASMSlowBulletProjectile : public ASMProjectile
{
	GENERATED_BODY()

public:
	ASMSlowBulletProjectile();

protected:
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	virtual void NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	bool IsValidateTarget(AActor* InTarget);

	void ApplyEffect(AActor* TargetActor);
};
