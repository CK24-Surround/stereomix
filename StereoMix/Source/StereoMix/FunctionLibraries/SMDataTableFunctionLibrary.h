// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Data/SMCharacterType.h"
#include "SMDataTableFunctionLibrary.generated.h"

class USMGameInstance;
struct FSMCharacterStatsData;
struct FSMCharacterAttackData;
struct FSMCharacterSkillData;
struct FSMCharacterNoiseBreakData;

/**
 * 
 */
UCLASS()
class STEREOMIX_API USMDataTableFunctionLibrary : public UObject
{
	GENERATED_BODY()

public:
	static FSMCharacterStatsData* GetCharacterStatData(ESMCharacterType CharacterType);

	static FSMCharacterAttackData* GetCharacterAttackData(ESMCharacterType CharacterType);

	static FSMCharacterSkillData* GetCharacterSkillData(ESMCharacterType CharacterType);

	static FSMCharacterNoiseBreakData* GetCharacterNoiseBreakData(ESMCharacterType CharacterType);

protected:
	static USMGameInstance* GetGameInstance();
};
