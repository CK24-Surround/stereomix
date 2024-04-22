// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "API/Matchmaking.h"
#include "Blueprint/UserWidget.h"
#include "SMMatchmakingWidget.generated.h"

DECLARE_LOG_CATEGORY_CLASS(LogMatchmakingUI, Log, All)

/**
 *
 */
UCLASS()
class STEREOMIX_API USMMatchmakingWidget : public UUserWidget
{
	GENERATED_BODY()

	FTimerHandle MatchStatusTimerHandle;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FString MatchId;

public:
	UPROPERTY(BlueprintAssignable)
	FOnRequestMatchCompleteDelegate OnRequestMatchComplete;

	UPROPERTY(BlueprintAssignable)
	FOnGetMatchStatusCompleteDelegate OnGetMatchStatusComplete;

	virtual void NativeOnInitialized() override;
	virtual void NativeDestruct() override;

	UFUNCTION(BlueprintCallable)
	void StartMatchmaking(const FString& PlayerName);

	UFUNCTION(BlueprintNativeEvent)
	void HandleRequestMatchComplete(bool bSuccess, const FRequestMatchmakingResponse& Response);

	UFUNCTION(BlueprintNativeEvent)
	void HandleGetMatchStatusComplete(bool bSuccess, const FGetMatchStatusResponse& Response);

	void OnMatchStatusCheckTick() const;
};
