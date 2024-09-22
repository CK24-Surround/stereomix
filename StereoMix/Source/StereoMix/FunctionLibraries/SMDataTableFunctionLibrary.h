// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMFunctionLibraryBase.h"
#include "Data/SMCharacterType.h"
#include "SMDataTableFunctionLibrary.generated.h"

struct FSMCharacterStatsData;
struct FSMCharacterAttackData;
struct FSMCharacterSkillData;
struct FSMCharacterNoiseBreakData;

/**
 * 
 */
UCLASS()
class STEREOMIX_API USMDataTableFunctionLibrary : public USMFunctionLibraryBase
{
	GENERATED_BODY()

public:
	static FSMCharacterStatsData* GetCharacterStatData(ESMCharacterType CharacterType);

	static FSMCharacterAttackData* GetCharacterAttackData(ESMCharacterType CharacterType);

	static FSMCharacterSkillData* GetCharacterSkillData(ESMCharacterType CharacterType);

	static FSMCharacterNoiseBreakData* GetCharacterNoiseBreakData(ESMCharacterType CharacterType);
};
