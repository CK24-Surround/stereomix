// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMClientAuthSubsystem.h"
#include "Backend/SMLobbySubsystem.h"
#include "SMClientLobbySubsystem.generated.h"

//TODO: gRPC 프로토버퍼에서 구현해서 백엔드에서 바로 넘겨주기

UENUM(BlueprintType)
enum class ECreateRoomResult : uint8
{
	Success,
	Cancelled,
	UnknownError,
	InternalError,
	DeadlineExceeded,
	Unauthenticated,
	InvalidArgument,
	ConnectionError,
};

UENUM(BlueprintType)
enum class EQuickMatchResult : uint8
{
	Success,
	Cancelled,
	UnknownError,
	InternalError,
	DeadlineExceeded,
	Unauthenticated,
	InvalidArgument,
	ConnectionError,
};

UENUM(BlueprintType)
enum EJoinRoomResult : uint8
{
	Success,
	Cancelled,
	UnknownError,
	InternalError,
	DeadlineExceeded,
	Unauthenticated,
	ConnectionError,
	InvalidArgument,
	InvalidPassword,
	RoomNotFound,
	RoomFull,
};

UENUM(BlueprintType)
enum class EJoinRoomWithCodeResult : uint8
{
	Success,
	Cancelled,
	UnknownError,
	InternalError,
	DeadlineExceeded,
	Unauthenticated,
	ConnectionError,
	InvalidArgument,
	RoomNotFound,
	RoomFull,
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCreateRoomResponseDelegate, ECreateRoomResult, Result, const FString&, ServerUrl);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FQuickMatchResponseDelegate, EQuickMatchResult, Result, const FString&, ServerUrl);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FJoinRoomResponseDelegate, EJoinRoomResult, Result, const FString&, ServerUrl);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FJoinRoomWithCodeResponseDelegate, EJoinRoomWithCodeResult, Result, const FString&, ServerUrl);

/**
 * StereoMix Lobby Subsystem for Client
 */
UCLASS()
class STEREOMIX_API USMClientLobbySubsystem : public USMLobbySubsystem
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FCreateRoomResponseDelegate OnCreateRoomResponse;

	UPROPERTY(BlueprintAssignable)
	FQuickMatchResponseDelegate OnQuickMatchResponse;

	UPROPERTY(BlueprintAssignable)
	FJoinRoomResponseDelegate OnJoinRoomResponse;

	UPROPERTY(BlueprintAssignable)
	FJoinRoomWithCodeResponseDelegate OnJoinRoomWithCodeResponse;

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/**
	 * 
	 * @param RoomName 방 이름
	 * @param RoomConfig 방 설정
	 * @param Password 방 비밀번호
	 * @return 요청이 성공적으로 시작되었는지 여부를 반환합니다. 서브시스템이 아직 초기화되지 않았거나 계정정보가 올바르지 않은 경우, 다른 요청이 이미 진행 중인 경우 false를 반환합니다.
	 */
	UFUNCTION(BlueprintCallable)
	bool CreateRoom(const FString& RoomName, FGrpcLobbyRoomConfig RoomConfig, const FString& Password = TEXT(""));

	UFUNCTION(BlueprintCallable)
	bool QuickMatch();

	UFUNCTION(BlueprintCallable)
	bool JoinRoom(const FString& RoomId, const FString& Password = TEXT(""));

	UFUNCTION(BlueprintCallable)
	bool JoinRoomWithCode(const FString& RoomCode);

	UFUNCTION(BlueprintCallable)
	void Cancel() const;

	virtual bool IsBusy() const override;

protected:
	FGrpcContextHandle GrpcContextHandle;

	UPROPERTY()
	TObjectPtr<USMClientAuthSubsystem> ClientAuthService;

	UPROPERTY()
	TObjectPtr<ULobbyServiceClient> LobbyClient;

	bool IsAuthenticated() const;

	UFUNCTION()
	void OnGrpcCreateRoomResponse(FGrpcContextHandle Handle, const FGrpcResult& GrpcResult, const FGrpcLobbyCreateRoomResponse& Response);

	UFUNCTION()
	void OnGrpcQuickMatchResponse(FGrpcContextHandle Handle, const FGrpcResult& GrpcResult, const FGrpcLobbyQuickMatchResponse& Response);

	UFUNCTION()
	void OnGrpcJoinRoomResponse(FGrpcContextHandle Handle, const FGrpcResult& GrpcResult, const FGrpcLobbyJoinRoomResponse& Response);

	UFUNCTION()
	void OnGrpcJoinRoomWithCodeResponse(FGrpcContextHandle Handle, const FGrpcResult& GrpcResult, const FGrpcLobbyJoinRoomWithCodeResponse& Response);

private:
	static FString GetServerUrl(const FGrpcLobbyRoomConnectionInfo& ConnectionInfo);
};
