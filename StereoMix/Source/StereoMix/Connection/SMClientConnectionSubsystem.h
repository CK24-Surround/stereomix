// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TurboLinkGrpcClient.h"
#include "SMConnectionSubsystem.h"
#include "SAuth/AuthMessage.h"
#include "SLobby/LobbyMessage.h"
#include "SLobby/LobbyService.h"
#include "SMClientConnectionSubsystem.generated.h"

// DECLARE_EVENT_OneParam(USMClientConnectionSubsystem, FLoginEvent, EGrpcResultCode /*Result*/)
// DECLARE_EVENT_ThreeParams(USMClientConnectionSubsystem, FCreateRoomEvent, EGrpcResultCode /*Result*/, EGrpcLobbyRoomDeploymentStatus /*DeployStatus*/, const FString& /*ConnectionUrl*/)
// DECLARE_EVENT_TwoParams(USMClientConnectionSubsystem, FJoinRoomEvent, EGrpcResultCode /*Result*/, const FString& /*ConnectionUrl*/)

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLoginEvent, EGrpcResultCode, Result);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FCreateRoomEvent, EGrpcResultCode, Result, EGrpcLobbyRoomDeploymentStatus, DeployStatus, const FString&, ConnectionUrl);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FJoinRoomEvent, EGrpcResultCode, Result, const FString&, ConnectionUrl);

USTRUCT(Blueprintable)
struct STEREOMIX_API FUserAccount
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FText UserName;
};

/**
 * StereoMix Client Connection Subsystem
 */
UCLASS(Config=Game, DefaultConfig)
class STEREOMIX_API USMClientConnectionSubsystem : public USMConnectionSubsystem
{
	GENERATED_BODY()

public:
	USMClientConnectionSubsystem();

	// ===============================================================
	// UGameInstanceSubsystem Interface

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;


	// ===============================================================
	// User Account
	
private:
	UPROPERTY()
	FUserAccount Account;

public:
	UFUNCTION(BlueprintCallable)
	const FUserAccount& GetAccount() const { return Account; }
	

	// ===============================================================
	// gRPC Lobby Service
	UPROPERTY(BlueprintAssignable)
	FGrpcServiceStateChangedEvent LobbyServiceStateChangedEvent;

	UFUNCTION(BlueprintCallable)
	void ConnectLobbyService() const;

protected:
	UPROPERTY(Transient)
	TObjectPtr<ULobbyService> LobbyService;

	UPROPERTY(Transient)
	TObjectPtr<ULobbyServiceClient> LobbyServiceClient;

	UFUNCTION()
	virtual void OnLobbyServiceStateChanged(EGrpcServiceState ServiceState);


	// ===============================================================
	// General Login

public:
	/**
	 * 로그인 결과를 알려주는 이벤트입니다.
	 */
	UPROPERTY(BlueprintAssignable, Category="Event")
	FLoginEvent LoginEvent;


	// ===============================================================
	// Guest Login
	/**
	 * 게스트 계정으로 로그인합니다.
	 * @param UserName 게스트 이름
	 */
	void GuestLogin(const FText& UserName);

private:
	FGrpcContextHandle GuestLoginHandle;

	UFUNCTION()
	void OnGuestLoginResponse(FGrpcContextHandle Handle, const FGrpcResult& Result, const FGrpcAuthLoginResponse& Response);


	// ===============================================================
	// Lobby CreateRoom
public:
	/**
	 * 방 생성 진행도 및 결과를 알려주는 이벤트입니다. 방 생성이 완료되면 ConnectionUrl을 제공합니다.
	 */
	UPROPERTY(BlueprintAssignable, Category="Event")
	FCreateRoomEvent CreateRoomEvent;

	/**
	 * 사용자 지정 방을 생성합니다.
	 * @param RoomName 방 이름
	 * @param Visibility 방 공개 여부
	 * @param GameMode 모드
	 * @param Map 맵
	 * @param Password 비밀번호
	 */
	void CreateRoom(const FString& RoomName, EGrpcLobbyRoomVisibility Visibility, EGrpcLobbyGameMode GameMode, EGrpcLobbyGameMap Map, const FString& Password = TEXT(""));

private:
	FGrpcContextHandle CreateRoomHandle;

	UFUNCTION()
	void OnCreateRoomResponse(FGrpcContextHandle Handle, const FGrpcResult& Result, const FGrpcLobbyCreateRoomResponse& Response);


	// ===============================================================
	// Lobby JoinRoom

public:
	/**
	 * 방 참가 결과를 알려주는 이벤트입니다. 방 참가에 성공하면 ConnectionUrl을 제공합니다.
	 */
	UPROPERTY(BlueprintAssignable, Category="Event")
	FJoinRoomEvent JoinRoomEvent;

	/**
	 * 방에 참가합니다.
	 * @param RoomId 방 ID
	 * @param Password 비밀번호
	 */
	void JoinRoom(const FString& RoomId, const FString& Password = TEXT(""));

	/**
	 * 방 코드를 사용하여 방에 참가합니다.
	 * @param ShortCode 방 단축 코드
	 */
	void JoinRoomUsingShortCode(const FString& ShortCode);

private:
	FGrpcContextHandle JoinRoomHandle;

	void JoinRoomInternal(const FGrpcLobbyJoinRoomRequest& Request);

	UFUNCTION()
	void OnJoinRoomResponse(FGrpcContextHandle Handle, const FGrpcResult& Result, const FGrpcLobbyJoinRoomResponse& Response);
};
