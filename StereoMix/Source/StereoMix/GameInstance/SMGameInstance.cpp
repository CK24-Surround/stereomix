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
		UE_LOG(LogStereoMix, Warning, TEXT("Custom mode on"))
	}

	if (bDemoGame)
	{
		UE_LOG(LogStereoMix, Warning, TEXT("Demo mode on"))
	}

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
	Http = &FHttpModule::Get();

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &ThisClass::ReceivedDataTableFromServer);

	const FString RequestURL;
	if (!RequestURL.IsEmpty())
	{
		Request->SetURL(RequestURL);

		Request->ProcessRequest();
		NET_LOG(GetPrimaryPlayerController(), Warning, TEXT("Call HTTP URL: %s"), *Request->GetURL());
	}
}

FSMCharacterStatsData* USMGameInstance::GetCharacterStatsData(ESMCharacterType CharacterType)
{
	return CharacterStats->FindRow<FSMCharacterStatsData>(CharacterTypeToName(CharacterType), TEXT(""));
}

FSMCharacterSkillData* USMGameInstance::GetCharacterSkillData(ESMCharacterType CharacterType)
{
	return CharacterSkill->FindRow<FSMCharacterSkillData>(CharacterTypeToName(CharacterType), TEXT(""));
}

FSMCharacterNoiseBreakData* USMGameInstance::GetCharacterNoiseBreakData(ESMCharacterType CharacterType)
{
	return CharacterNoiseBreak->FindRow<FSMCharacterNoiseBreakData>(CharacterTypeToName(CharacterType), TEXT(""));
}

void USMGameInstance::ReceivedDataTableFromServer(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<TCHAR>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		FString Field = TEXT("Character");
		TSharedPtr<FJsonObject> JsonData = JsonObject->GetArrayField(Field)[0]->AsObject();
		Field = TEXT("CharacterName");
		const FString JsonName = JsonData->GetStringField(Field);
		NET_LOG(GetPrimaryPlayerController(), Warning, TEXT("데이터 테이블 리시브"));
	}
}

FName USMGameInstance::CharacterTypeToName(ESMCharacterType CharacterType)
{
	FName RowName;
	switch (CharacterType)
	{
		case ESMCharacterType::ElectricGuitar:
		{
			RowName = TEXT("ElectricGuitar");
			break;
		}
		case ESMCharacterType::Piano:
		{
			RowName = TEXT("Piano");
			break;
		}
		case ESMCharacterType::Bass:
		{
			RowName = TEXT("Bass");
			break;
		}
		default:
		{
			break;
		}
	}

	return RowName;
}
