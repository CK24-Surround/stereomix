// Copyright Studio Surround. All Rights Reserved.


#include "SMDataTableFunctionLibrary.h"

#include "GameInstance/SMGameInstance.h"
#include "Utilities/SMLog.h"


FSMCharacterStatsData* USMDataTableFunctionLibrary::GetCharacterStatData(ESMCharacterType CharacterType)
{
	USMGameInstance* GameInstance = GetGameInstance();
	return GameInstance ? GameInstance->GetCharacterStatsData(CharacterType) : nullptr;
}

FSMCharacterAttackData* USMDataTableFunctionLibrary::GetCharacterAttackData(ESMCharacterType CharacterType)
{
	USMGameInstance* GameInstance = GetGameInstance();
	return GameInstance ? GameInstance->GetCharacterAttackData(CharacterType) : nullptr;
}

FSMCharacterSkillData* USMDataTableFunctionLibrary::GetCharacterSkillData(ESMCharacterType CharacterType)
{
	USMGameInstance* GameInstance = GetGameInstance();
	return GameInstance ? GameInstance->GetCharacterSkillData(CharacterType) : nullptr;
}

FSMCharacterNoiseBreakData* USMDataTableFunctionLibrary::GetCharacterNoiseBreakData(ESMCharacterType CharacterType)
{
	USMGameInstance* GameInstance = GetGameInstance();
	return GameInstance ? GameInstance->GetCharacterNoiseBreakData(CharacterType) : nullptr;
}

USMGameInstance* USMDataTableFunctionLibrary::GetGameInstance()
{
	if (!GEngine)
	{
		return nullptr;
	}

	USMGameInstance* GameInstance = nullptr;
	for (const FWorldContext& WorldContext : GEngine->GetWorldContexts())
	{
		if (USMGameInstance* CachedGameInstance = Cast<USMGameInstance>(WorldContext.OwningGameInstance))
		{
			GameInstance = CachedGameInstance;
			break;
		}
	}

	return GameInstance;
}
