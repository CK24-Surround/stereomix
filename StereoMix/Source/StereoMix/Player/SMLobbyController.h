// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SAuth/AuthService.h"
#include "SLobby/LobbyService.h"
#include "UI/LobbyUserWidget.h"
#include "SMLobbyController.generated.h"

class USMGrpcSubsystem;

DECLARE_LOG_CATEGORY_CLASS(LogSMLobbyController, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLoginResponse, bool, Success);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCreateRoomResponse, bool, Success, const FGrpcLobbyConnectionInfo&, ConnectionInfo);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnJoinRoomResponse, bool, Success, const FGrpcLobbyConnectionInfo&, ConnectionInfo);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRoomListUpdateResponse, bool, Success, const TArray<FGrpcLobbyRoomInfo>&, RoomList);

/**
 * PlayerController for Lobby
 */
UCLASS()
class STEREOMIX_API ASMLobbyController : public APlayerController
{
	GENERATED_BODY()

	// UI Section

	UPROPERTY(Transient)
	TObjectPtr<UUserWidget> LoadingScreenInstance;

	UPROPERTY(Transient)
	TObjectPtr<ULobbyUserWidget> CurrentWidget;

	// Grpc Section

	UPROPERTY(Transient)
	TObjectPtr<USMGrpcSubsystem> GrpcSubsystem;

	UPROPERTY(Transient)
	TObjectPtr<UAuthService> AuthService;

	UPROPERTY(Transient)
	TObjectPtr<UAuthServiceClient> AuthServiceClient;

	UPROPERTY(Transient)
	TObjectPtr<ULobbyService> LobbyService;

	UPROPERTY(Transient)
	TObjectPtr<ULobbyServiceClient> LobbyServiceClient;

	bool bUserTokenReceived = false;
	FString UserToken;
	FGrpcMetaData UserMetaData;

	void InitAuthService();
	void InitLobbyService();

	UFUNCTION()
	void OnAuthServiceStateChanged(EGrpcServiceState State);

	UFUNCTION()
	void OnLobbyServiceStateChanged(EGrpcServiceState State);

	FGrpcContextHandle GuestLoginContext;

	UFUNCTION()
	void HandleGuestLoginResponse(FGrpcContextHandle ContextHandle, const FGrpcResult& Result, const FGrpcAuthResponse& Response);

	FGrpcContextHandle CreateRoomContext;

	UFUNCTION()
	void HandleCreateRoomResponse(FGrpcContextHandle ContextHandle, const FGrpcResult& Result, const FGrpcLobbyConnectionInfo& Response);

	FGrpcContextHandle JoinRoomContext;

	UFUNCTION()
	void HandleJoinRoomResponse(FGrpcContextHandle ContextHandle, const FGrpcResult& Result, const FGrpcLobbyConnectionInfo& Response);

	FGrpcContextHandle GetRoomListContext;

	UFUNCTION()
	void HandleGetRoomListResponse(FGrpcContextHandle ContextHandle, const FGrpcResult& Result, const FGrpcLobbyGetRoomListResponse& Response);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> LoadingScreenUI;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> LobbyUI;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> RoomUI;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Response")
	FOnLoginResponse OnLoginResponse;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Response")
	FOnCreateRoomResponse OnCreateRoomResponse;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Response")
	FOnJoinRoomResponse OnJoinRoomResponse;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Response")
	FOnRoomListUpdateResponse OnRoomListUpdateResponse;

	ASMLobbyController();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// Loading Screen
	UFUNCTION(BlueprintCallable)
	void ShowLoadingScreen();

	UFUNCTION(BlueprintCallable)
	void CloseLoadingScreen();

	// Auth
	UFUNCTION(BlueprintCallable)
	void GuestLogin();

	// Lobby
	UFUNCTION(BlueprintCallable)
	void CreateRoom(const FString& RoomName, EGrpcLobbyRoomVisibility Visibility = EGrpcLobbyRoomVisibility::ROOM_VISIBILITY_PUBLIC, EGrpcLobbyGameMode Mode = EGrpcLobbyGameMode::GAMEMODE_DEFAULT, EGrpcLobbyGameMap Map = EGrpcLobbyGameMap::GAMEMAP_DEFAULT, const FString& Password = TEXT(""));

	UFUNCTION(BlueprintCallable)
	void JoinRoom(const FString& RoomId, const FString& Password = TEXT(""));

	UFUNCTION(BlueprintCallable)
	void GetRoomList(EGrpcLobbyRoomVisibility Visibility = EGrpcLobbyRoomVisibility::ROOM_VISIBILITY_PUBLIC, EGrpcLobbyGameMode Mode = EGrpcLobbyGameMode::GAMEMODE_DEFAULT, EGrpcLobbyGameMap Map = EGrpcLobbyGameMap::GAMEMAP_DEFAULT);

	UFUNCTION(BlueprintNativeEvent)
	void OnReceiveLoginResponse(bool Success);

	UFUNCTION(BlueprintNativeEvent)
	void OnReceiveCreateRoomResponse(bool Success, const FGrpcLobbyConnectionInfo& ConnectionInfo);

	UFUNCTION(BlueprintNativeEvent)
	void OnReceiveJoinRoomResponse(bool Success, const FGrpcLobbyConnectionInfo& ConnectionInfo);

	UFUNCTION(BlueprintNativeEvent)
	void OnReceiveRoomListUpdateResponse(bool Success, const TArray<FGrpcLobbyRoomInfo>& RoomList);
};
