// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameFlowSubsystem.generated.h"

class USMUserWidget_LoadingScreen;

UENUM(BlueprintType)
enum class ELoginScreenType : uint8
{
	InGame,
	OutGame
};

/**
 * 
 */
UCLASS(BlueprintType, Config=Game, DefaultConfig)
class STEREOMIX_API UGameFlowSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UGameFlowSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;

private:
	UPROPERTY(Config)
	TSubclassOf<UUserWidget> InGameLoadingScreenClass;

	UPROPERTY(Config)
	TSubclassOf<UUserWidget> OutGameLoadingScreenClass;

	UPROPERTY(Transient)
	TObjectPtr<USMUserWidget_LoadingScreen> InGameLoadingScreenWidget;

	UPROPERTY(Transient)
	TObjectPtr<USMUserWidget_LoadingScreen> OutGameLoadingScreenWidget;
};
