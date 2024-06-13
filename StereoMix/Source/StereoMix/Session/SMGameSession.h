// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameSession.h"
#include "SMGameSession.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogSMGameSession, Log, All)

UCLASS()
class STEREOMIX_API ASMGameSession : public AGameSession
{
	GENERATED_BODY()

public:
	UPROPERTY()
	bool bCanEnterRoom;

	// Sets default values for this actor's properties
	ASMGameSession();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// 게임 세션 초기화
	virtual void InitOptions(const FString& Options) override;

	// 온라인 서비스에 서버를 등록할 때 사용
	virtual void RegisterServer() override;

	// 로그인을 승인할 때 사용
	virtual FString ApproveLogin(const FString& Options) override;

	// 로그인에 성공한 플레이어를 처리할 때 사용
	virtual void PostLogin(APlayerController* NewPlayer) override;

	// 온라인 서비스에 플레이어를 등록할 때 사용
	virtual void RegisterPlayer(APlayerController* NewPlayer, const FUniqueNetIdRepl& UniqueId, bool bWasFromInvite) override;

	// 로그아웃을 처리할 때 사용
	virtual void NotifyLogout(const APlayerController* PC) override;

	// 온라인 서비스에서 플레이어를 제거할 때 사용
	virtual void UnregisterPlayer(const APlayerController* ExitingPlayer) override;


	virtual void PostSeamlessTravel() override;

	virtual bool HandleStartMatchRequest() override;

	virtual void HandleMatchIsWaitingToStart() override;

	virtual void HandleMatchHasStarted() override;

	virtual void HandleMatchHasEnded() override;
};
