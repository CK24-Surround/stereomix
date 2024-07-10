// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Games/Room/SMRoomPlayerState.h"
#include "SMPlayerController.h"
#include "UI/Widget/Loading/SMLoadingScreenWidget.h"
#include "UI/Widget/Room/SMRoomWidget.h"

#include "SMRoomPlayerController.generated.h"

/**
 *
 */
UCLASS()
class STEREOMIX_API ASMRoomPlayerController : public ASMPlayerController
{
	GENERATED_BODY()

public:
	ASMRoomPlayerController();

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void PreClientTravel(const FString& PendingURL, ETravelType TravelType, bool bIsSeamlessTravel) override;

	void OnCompleteLoading();

	virtual void OnRep_PlayerState() override;

	ASMRoomPlayerState* GetRoomPlayerState() const { return RoomPlayerState.Get(); }

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void RequestImmediateStartGame();

protected:
	UFUNCTION()
	void OnTeamChanged(ESMTeam NewTeam);

	UPROPERTY(EditDefaultsOnly, Category = "Widget")
	TSubclassOf<USMLoadingScreenWidget> LoadingScreenWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Widget")
	TSubclassOf<USMRoomWidget> RoomWidgetClass;

	UPROPERTY(Transient)
	TObjectPtr<USMLoadingScreenWidget> LoadingScreenWidget;

	UPROPERTY(Transient)
	TObjectPtr<USMRoomWidget> RoomWidget;

	UPROPERTY(Transient)
	TWeakObjectPtr<ASMRoomState> RoomState;

	UPROPERTY(Transient)
	TWeakObjectPtr<ASMRoomPlayerState> RoomPlayerState;

	bool bTravelingToCharacterSelect = false;
};
