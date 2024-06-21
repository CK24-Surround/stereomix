// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameSession.h"
#include "SLobby/LobbyClient.h"
#include "Subsystem/SMServerRoomSubsystem.h"
#include "SMGameSession.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogSMGameSession, Log, All)

UCLASS()
class STEREOMIX_API ASMGameSession : public AGameSession
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASMGameSession();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	bool IsValidRoom() const;

	void UpdateRoomState(EGrpcLobbyRoomState NewState);

	void DeleteRoom();

	void SetCanEnterRoom(bool bCanEnter);

protected:
	// 게임 세션 초기화
	virtual void InitOptions(const FString& Options) override;

	// 온라인 서비스에 서버를 등록할 때 사용
	virtual void RegisterServer() override;

	// 로그인을 승인할 때 사용
	virtual FString ApproveLogin(const FString& Options) override;

	// 로그인에 성공한 플레이어를 처리할 때 사용
	virtual void PostLogin(APlayerController* NewPlayer) override;

	// 로그아웃을 처리할 때 사용
	virtual void NotifyLogout(const APlayerController* PC) override;

	UPROPERTY()
	bool bCanEnterRoom;

	UPROPERTY()
	TObjectPtr<USMServerRoomSubsystem> RoomSubsystem;

	UPROPERTY()
	TObjectPtr<ULobbyServiceClient> LobbyServiceClient;
};