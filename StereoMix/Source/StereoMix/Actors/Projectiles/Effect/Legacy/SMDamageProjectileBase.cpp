// Copyright Surround, Inc. All Rights Reserved.


#include "SMDamageProjectileBase.h"


void ASMDamageProjectileBase::HandleHitEffect(AActor* InTarget)
{
	Super::HandleHitEffect(InTarget);

	ApplyDamage(InTarget);
}
