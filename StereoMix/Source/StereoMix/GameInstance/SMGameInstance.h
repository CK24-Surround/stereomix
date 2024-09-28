// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/SMCharacterType.h"
#include "Interfaces/IHttpRequest.h"

#include "SMGameInstance.generated.h"

struct FSMCharacterNoiseBreakData;
struct FSMCharacterSkillData;
struct FSMCharacterAttackData;
struct FSMCharacterStatsData;
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

	FSMCharacterStatsData* GetCharacterStatsData(ESMCharacterType CharacterType);

	FSMCharacterAttackData* GetCharacterAttackData(ESMCharacterType CharacterType);

	FSMCharacterSkillData* GetCharacterSkillData(ESMCharacterType CharacterType);

	FSMCharacterNoiseBreakData* GetCharacterNoiseBreakData(ESMCharacterType CharacterType);

protected:
	void ReceivedDataTableFromServer(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);

	FName CharacterTypeToName(ESMCharacterType CharacterType);

	FHttpModule* Http;

	UPROPERTY(EditAnywhere, Category = "Design")
	TObjectPtr<UDataTable> CharacterStatsDataTable;

	UPROPERTY(EditAnywhere, Category = "Design")
	TObjectPtr<UDataTable> CharacterAttackDataTable;

	UPROPERTY(EditAnywhere, Category = "Design")
	TObjectPtr<UDataTable> CharacterSkillDataTable;

	UPROPERTY(EditAnywhere, Category = "Design")
	TObjectPtr<UDataTable> CharacterNoiseBreakDataTable;

private:
	bool bDemoGame = false;
	bool bCustomGame = false;
	bool bViewedSplashScreen = false;
};
