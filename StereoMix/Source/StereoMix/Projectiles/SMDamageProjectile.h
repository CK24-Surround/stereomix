// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMProjectile.h"
#include "SMDamageProjectile.generated.h"

UCLASS()
class STEREOMIX_API ASMDamageProjectile : public ASMProjectile
{
	GENERATED_BODY()

public:
	ASMDamageProjectile();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

protected:
	/** 오버렙 판정 발생 시 호출됩니다. 보통 플레이어와 충돌할때 호출됩니다.*/
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	/** 블로킹 판정 발생 시 호출됩니다. 보통 벽에 부딪힐때 호출됩니다. */
	virtual void NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

protected:
	virtual bool IsValidateTarget(AActor* InTarget) override;

	/** GE를 통해 데미지를 적용합니다. */
	void ApplyDamage(AActor* InTarget);

	/** GC를 통해 FX를 적용합니다. */
	void ApplyFX(AActor* InTarget);
};
