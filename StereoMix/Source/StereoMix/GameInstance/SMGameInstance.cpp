// Copyright Surround, Inc. All Rights Reserved.

#include "SMGameInstance.h"

#include "HttpModule.h"
#include "StereoMixLog.h"
#include "Data/DataTable/SMCharacterData.h"
#include "Interfaces/IHttpResponse.h"
#include "Settings/SMGameUserSettings.h"
#include "Utilities/SMLog.h"

void USMGameInstance::Init()
{
	const TCHAR* GameArgs = FCommandLine::Get();
	UE_LOG(LogStereoMix, Warning, TEXT("GameArgs: %s"), GameArgs);
	bCustomGame = FParse::Param(GameArgs, TEXT("custom"));

	if (!bCustomGame)
	{
		bDemoGame = FParse::Param(GameArgs, TEXT("demo"));
	}

	if (bCustomGame)
	{
		UE_LOG(LogStereoMix, Warning, TEXT("Custom mode on"));
	}

	if (bDemoGame)
	{
		UE_LOG(LogStereoMix, Warning, TEXT("Demo mode on"));
	}

	RequestDataTableToServer();

	Super::Init();
}

void USMGameInstance::StartGameInstance()
{
	Super::StartGameInstance();
}

void USMGameInstance::OnStart()
{
	Super::OnStart();

	USMGameUserSettings* GameUserSettings = USMGameUserSettings::GetStereoMixUserSettings();
	GameUserSettings->LoadSettings();
	GameUserSettings->ApplySettings(true);
}

FString USMGameInstance::GetGameVersion()
{
	FString GameVersion;
	GConfig->GetString(TEXT("/Script/EngineSettings.GeneralProjectSettings"), TEXT("ProjectVersion"), GameVersion, GGameIni);
	return GameVersion;
}

bool USMGameInstance::IsCustomGame() const
{
	return bCustomGame;
}

bool USMGameInstance::IsDemoGame() const
{
	return bDemoGame;
}

void USMGameInstance::RequestDataTableToServer()
{
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();

	Request->OnProcessRequestComplete().BindUObject(this, &ThisClass::ReceivedDataTableFromServer);

	UE_LOG(LogStereoMix, Log, TEXT("Requesting DataTable from Server"));
	const FString RequestURL = "https://stereomix-502920527569.asia-northeast3.run.app/stats/character";
	if (!RequestURL.IsEmpty())
	{
		Request->SetURL(RequestURL);
		Request->ProcessRequest();
	}
}

FSMCharacterStatsData* USMGameInstance::GetCharacterStatsData(ESMCharacterType CharacterType)
{
	return CharacterStatsDataTable->FindRow<FSMCharacterStatsData>(CharacterTypeToName(CharacterType), TEXT(""));
}

FSMCharacterAttackData* USMGameInstance::GetCharacterAttackData(ESMCharacterType CharacterType)
{
	return CharacterAttackDataTable->FindRow<FSMCharacterAttackData>(CharacterTypeToName(CharacterType), TEXT(""));
}

FSMCharacterSkillData* USMGameInstance::GetCharacterSkillData(ESMCharacterType CharacterType)
{
	return CharacterSkillDataTable->FindRow<FSMCharacterSkillData>(CharacterTypeToName(CharacterType), TEXT(""));
}

FSMCharacterNoiseBreakData* USMGameInstance::GetCharacterNoiseBreakData(ESMCharacterType CharacterType)
{
	return CharacterNoiseBreakDataTable->FindRow<FSMCharacterNoiseBreakData>(CharacterTypeToName(CharacterType), TEXT(""));
}

template<typename DataType, typename FieldType>
void USMGameInstance::UpdateDataTable(TSharedPtr<FJsonObject> CharacterData, UDataTable* DataTable, const FString& CharacterName, const TMap<FString, FieldType DataType::*>& FieldMap)
{
	DataType* Data = DataTable->FindRow<DataType>(FName(CharacterName), TEXT(""));
	if (CharacterData.IsValid() && Data)
	{
		for (const auto& Field : FieldMap)
		{
			if (CharacterData->HasField(Field.Key))
			{
				if constexpr (std::is_same_v<FieldType, float>)
				{
					Data->*(Field.Value) = CharacterData->GetNumberField(Field.Key);
				}
				else if constexpr (std::is_same_v<FieldType, int32>)
				{
					Data->*(Field.Value) = CharacterData->GetIntegerField(Field.Key);
				}
			}
		}
	}
}

void USMGameInstance::ReceivedDataTableFromServer(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		UE_LOG(LogStereoMix, Log, TEXT("DataTable received from Server"));

		for (auto& CharacterItem : JsonObject->Values)
		{
			FString CharacterName = CharacterItem.Key;
			TSharedPtr<FJsonObject> CharacterData = CharacterItem.Value->AsObject();

			if (CharacterData.IsValid())
			{
				// Stats Data
				TMap<FString, float FSMCharacterStatsData::*> StatsFieldMap = {
					{ TEXT("HP"), &FSMCharacterStatsData::HP },
					{ TEXT("MoveSpeed"), &FSMCharacterStatsData::MoveSpeed },
					{ TEXT("Stamina"), &FSMCharacterStatsData::Stamina },
					{ TEXT("SkillGauge"), &FSMCharacterStatsData::SkillGauge }
				};
				UpdateDataTable<FSMCharacterStatsData, float>(CharacterData->GetObjectField(TEXT("Stats")), CharacterStatsDataTable, CharacterName, StatsFieldMap);

				// Attack Data
				TMap<FString, float FSMCharacterAttackData::*> AttackFieldMap = {
					{ TEXT("Damage"), &FSMCharacterAttackData::Damage },
					{ TEXT("DistanceByTile"), &FSMCharacterAttackData::DistanceByTile },
					{ TEXT("ProjectileSpeed"), &FSMCharacterAttackData::ProjectileSpeed },
					{ TEXT("AttackPerSecond"), &FSMCharacterAttackData::AttackPerSecond },
					{ TEXT("SpreadAngle"), &FSMCharacterAttackData::SpreadAngle }
				};
				UpdateDataTable<FSMCharacterAttackData, float>(CharacterData->GetObjectField(TEXT("Attack")), CharacterAttackDataTable, CharacterName, AttackFieldMap);

				// Skill Data
				TMap<FString, float FSMCharacterSkillData::*> SkillFieldMap = {
					{ TEXT("Damage"), &FSMCharacterSkillData::Damage },
					{ TEXT("DistanceByTile"), &FSMCharacterSkillData::DistanceByTile },
					{ TEXT("ProjectileSpeed"), &FSMCharacterSkillData::ProjectileSpeed },
					{ TEXT("StartUpTime"), &FSMCharacterSkillData::StartUpTime },
					{ TEXT("Range"), &FSMCharacterSkillData::Range },
					{ TEXT("Magnitude"), &FSMCharacterSkillData::Magnitude },
					{ TEXT("Duration"), &FSMCharacterSkillData::Duration }
				};
				UpdateDataTable<FSMCharacterSkillData, float>(CharacterData->GetObjectField(TEXT("Skill")), CharacterSkillDataTable, CharacterName, SkillFieldMap);

				// Noise Break Data (float fields)
				TMap<FString, float FSMCharacterNoiseBreakData::*> NoiseBreakFieldMap = {
					{ TEXT("Damage"), &FSMCharacterNoiseBreakData::Damage },
					{ TEXT("DistanceByTile"), &FSMCharacterNoiseBreakData::DistanceByTile },
					{ TEXT("GravityScale"), &FSMCharacterNoiseBreakData::GravityScale },
					{ TEXT("ApexHeight"), &FSMCharacterNoiseBreakData::ApexHeight }
				};
				UpdateDataTable<FSMCharacterNoiseBreakData, float>(CharacterData->GetObjectField(TEXT("NoiseBreak")), CharacterNoiseBreakDataTable, CharacterName, NoiseBreakFieldMap);

				// Noise Break Data (int32 fields)
				TMap<FString, int32 FSMCharacterNoiseBreakData::*> NoiseBreakIntFieldMap = {
					{ TEXT("CaptureSize"), &FSMCharacterNoiseBreakData::CaptureSize }
				};
				UpdateDataTable<FSMCharacterNoiseBreakData, int32>(CharacterData->GetObjectField(TEXT("NoiseBreak")), CharacterNoiseBreakDataTable, CharacterName, NoiseBreakIntFieldMap);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("JSON 파싱 오류"));
	}
}

FName USMGameInstance::CharacterTypeToName(ESMCharacterType CharacterType)
{
	switch (CharacterType)
	{
		case ESMCharacterType::ElectricGuitar:
			return TEXT("ElectricGuitar");
		case ESMCharacterType::Piano:
			return TEXT("Piano");
		case ESMCharacterType::Bass:
			return TEXT("Bass");
		default:
			return NAME_None;
	}
}
