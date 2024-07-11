// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "SMGameInstance.generated.h"

/**
 *
 */
UCLASS()
class STEREOMIX_API USMGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;

	virtual void StartGameInstance() override;

	virtual void OnStart() override;

	static FString GetGameVersion();

	UFUNCTION(BlueprintCallable, Category = "Game")
	void SetViewedSplashScreen(const bool bViewed) { bViewedSplashScreen = bViewed; }

	UFUNCTION(BlueprintPure, Category = "Game")
	bool IsCustomGame() const;

	UFUNCTION(BlueprintPure, Category = "Game")
	bool IsDemoGame() const;

	UFUNCTION(BlueprintPure, Category = "Game")
	bool IsViewedSplashScreen() const { return bViewedSplashScreen; }

private:
	bool bDemoGame = false;
	bool bCustomGame = false;
	bool bViewedSplashScreen = false;
};
