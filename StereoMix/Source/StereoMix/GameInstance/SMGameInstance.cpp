// Copyright Surround, Inc. All Rights Reserved.


#include "SMGameInstance.h"

USMGameInstance::USMGameInstance()
{
}

void USMGameInstance::Init()
{
	Super::Init();
}

void USMGameInstance::StartGameInstance()
{
	Super::StartGameInstance();
}

FString USMGameInstance::GetGameVersion()
{
	FString GameVersion;
	GConfig->GetString(TEXT("/Script/EngineSettings.GeneralProjectSettings"), TEXT("ProjectVersion"), GameVersion, GGameIni);
	return GameVersion;
}
