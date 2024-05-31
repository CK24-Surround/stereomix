// Copyright Surround, Inc. All Rights Reserved.


#include "GameFlowSubsystem.h"

#include "StereoMix.h"
#include "Blueprint/UserWidget.h"

UGameFlowSubsystem::UGameFlowSubsystem()
{
}

void UGameFlowSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (!GetWorld())
	{
		UE_LOG(LogStereoMix, Error, TEXT("[GameFlowSubsystem] Initialize Failed: World is nullptr"))
		return;
	}

	UGameInstance* GameInstance = GetOuterUGameInstance();
	if (!GameInstance)
	{
		UE_LOG(LogStereoMix, Error, TEXT("[GameFlowSubsystem] Initialize Failed: GameInstance is nullptr"))
		return;
	}

	// Create Widget
	InGameLoadingScreenWidget = CreateWidget<USMUserWidget_LoadingScreen>(GameInstance, InGameLoadingScreenClass);
	if (!InGameLoadingScreenWidget)
	{
		UE_LOG(LogStereoMix, Error, TEXT("[GameFlowSubsystem] Initialize Failed: InGameLoadingScreen is nullptr"))
		return;
	}

	OutGameLoadingScreenWidget = CreateWidget<USMUserWidget_LoadingScreen>(GameInstance, OutGameLoadingScreenClass);
	if (!OutGameLoadingScreenWidget)
	{
		UE_LOG(LogStereoMix, Error, TEXT("[GameFlowSubsystem] Initialize Failed: OutGameLoadingScreen is nullptr"))
		return;
	}
}

void UGameFlowSubsystem::Deinitialize()
{
	Super::Deinitialize();
}
