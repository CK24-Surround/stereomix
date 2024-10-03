// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Projectiles/Effect/SMEffectProjectileBase.h"

#include "SMDamageProjectileBase.generated.h"

UCLASS(Abstract)
class STEREOMIX_API ASMDamageProjectileBase : public ASMEffectProjectileBase
{
	GENERATED_BODY()

protected:
	/* 대상에 적중했을때 처리할 효과들을 추가하고 싶다면 이 함수를 오버라이딩하면 됩니다. */
	virtual void HandleHitEffect(AActor* InTarget) override;
};
