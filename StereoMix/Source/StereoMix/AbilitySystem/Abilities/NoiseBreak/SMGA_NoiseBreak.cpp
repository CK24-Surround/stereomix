// Copyright Surround, Inc. All Rights Reserved.


#include "SMGA_NoiseBreak.h"

#include "AbilitySystem/SMTags.h"


USMGA_NoiseBreak::USMGA_NoiseBreak()
{
	ActivationOwnedTags = FGameplayTagContainer(SMTags::Character::State::NoiseBreaking);
	ActivationRequiredTags = FGameplayTagContainer(SMTags::Character::State::Hold);
}
