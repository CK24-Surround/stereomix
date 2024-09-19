// Copyright Surround, Inc. All Rights Reserved.


#include "SMElectricGuitarCharacter.h"

#include "AbilitySystem/SMTags.h"


ASMElectricGuitarCharacter::ASMElectricGuitarCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CharacterType = ESMCharacterType::ElectricGuitar;

	LockAimTags.AddTag(SMTags::Character::State::SlowBullet);

	LockMovementTags.AddTag(SMTags::Character::State::SlowBullet);
}
