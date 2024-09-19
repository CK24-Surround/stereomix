// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/SMGameplayAbility.h"
#include "Data/SMCharacterType.h"
#include "SMGA_Skill.generated.h"

struct FSMCharacterSkillData;

/**
 * 
 */
UCLASS(Abstract)
class STEREOMIX_API USMGA_Skill : public USMGameplayAbility
{
	GENERATED_BODY()

public:
	USMGA_Skill();

	FSMCharacterSkillData* GetSkillData(ESMCharacterType CharacterType);

protected:
	float Damage = 0.0f;

	float MaxDistanceByTile = 0.0f;
};
