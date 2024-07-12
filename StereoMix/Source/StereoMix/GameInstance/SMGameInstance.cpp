// Copyright Surround, Inc. All Rights Reserved.


#include "SMGameInstance.h"

#include "StereoMixLog.h"
#include "Settings/SMGameUserSettings.h"

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
