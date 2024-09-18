// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/SMCharacterType.h"
#include "Data/DataTable/SMCharacterData.h"
#include "Interfaces/IHttpRequest.h"

#include "SMGameInstance.generated.h"

class FHttpModule;

/**
 *
 */
UCLASS(Abstract)
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

	void RequestDataTableToServer();

	FSMCharacterStatData* GetCharacterStatData(ESMCharacterType CharacterType);

	FSMCharacterSkillData* GetCharacterSkillData(ESMCharacterType CharacterType);

	FSMCharacterNoiseBreakData* GetCharacterNoiseBreakData(ESMCharacterType CharacterType);

protected:
	void ReceivedDataTableFromServer(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);

	FName CharacterTypeToName(ESMCharacterType CharacterType);

	FHttpModule* Http;

	UPROPERTY(EditAnywhere, Category = "Design")
	TObjectPtr<UDataTable> CharacterStat;

	UPROPERTY(EditAnywhere, Category = "Design")
	TObjectPtr<UDataTable> CharacterSkill;

	UPROPERTY(EditAnywhere, Category = "Design")
	TObjectPtr<UDataTable> CharacterNoiseBreak;

private:
	bool bDemoGame = false;
	bool bCustomGame = false;
	bool bViewedSplashScreen = false;
};
