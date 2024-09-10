// Copyright Surround, Inc. All Rights Reserved.


#include "SMGA_Skill.h"

#include "AbilitySystem/SMTags.h"

USMGA_Skill::USMGA_Skill()
{
	ActivationBlockedTags.AddTag(SMTags::Character::State::Hold);
	ActivationBlockedTags.AddTag(SMTags::Character::State::Holded);
	ActivationBlockedTags.AddTag(SMTags::Character::State::NoiseBreaking);
	ActivationBlockedTags.AddTag(SMTags::Character::State::NoiseBreaked);
	ActivationBlockedTags.AddTag(SMTags::Character::State::Neutralize);
	ActivationBlockedTags.AddTag(SMTags::Character::State::Immune);
}
