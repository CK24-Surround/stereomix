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

	UFUNCTION(BlueprintPure, Category = "Game")
	bool IsCustomGame() const;

	UFUNCTION(BlueprintPure, Category = "Game")
	bool IsDemoGame() const;

private:
	bool bDemoGame = false;
	bool bCustomGame = false;
};
