// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMBaseHUD.h"
#include "TurboLinkGrpcClient.h"
#include "SLobby/LobbyMessage.h"
#include "UI/ViewModel/SMViewModel_Title.h"
#include "SMLobbyHUD.generated.h"

class USMViewModel_Lobby;
class USMViewModel_RoomShortCodePopup;
/**
 * 
 */
UCLASS()
class STEREOMIX_API ASMLobbyHUD : public ASMBaseHUD
{
	GENERATED_BODY()

public:
	ASMLobbyHUD();

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UUserWidget> JoinRoomWithCodeWidgetClass;

	UPROPERTY(Transient)
	TObjectPtr<UUserWidget> JoinRoomWithCodeWidget;
	
	UPROPERTY(Transient)
	TObjectPtr<USMViewModel_Lobby> LobbyViewModel;
	
	UPROPERTY(Transient)
	TObjectPtr<USMViewModel_RoomShortCodePopup> RoomShortCodePopupViewModel;

	void CreateRoomShortCodePopupWidget();

	UFUNCTION()
	void OnRequestJoinRoom();

	UFUNCTION()
	void OnJoinRoomWithCodeSubmit();

	UFUNCTION()
	void OnJoinRoomWithCodeCancel();
	
	UFUNCTION()
	void OnLobbyServiceStateChanged(EGrpcServiceState ChangedState);
	
	UFUNCTION()
	void OnCreateRoom(EGrpcResultCode Result, EGrpcLobbyRoomDeploymentStatus DeployStatus, const FString& ConnectionUrl);
	
	UFUNCTION()
	void OnJoinRoom(EGrpcResultCode Result, const FString& ConnectionUrl);
};
