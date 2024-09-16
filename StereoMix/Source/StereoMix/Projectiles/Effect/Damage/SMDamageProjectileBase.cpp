// Copyright Surround, Inc. All Rights Reserved.


#include "SMDamageProjectileBase.h"

#include "AbilitySystem/SMTags.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Utilities/SMLog.h"


ASMDamageProjectileBase::ASMDamageProjectileBase()
{
	IgnoreTargetStateTags.AddTag(SMTags::Character::State::Neutralize);
	IgnoreTargetStateTags.AddTag(SMTags::Character::State::Immune);
	IgnoreTargetStateTags.AddTag(SMTags::Character::State::NoiseBreak);
}

void ASMDamageProjectileBase::HandleHitEffect(AActor* InTarget)
{
	Super::HandleHitEffect(InTarget);

	ApplyDamage(InTarget);
}
