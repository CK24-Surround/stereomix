// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "Session/SMGameSession.h"

#include "SMGameMode.generated.h"

class ASMGameState;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStartMatchSignature);

/**
 *
 */
UCLASS()
class STEREOMIX_API ASMGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ASMGameMode();

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	virtual void OnPostLogin(AController* NewPlayer) override;

	virtual void Logout(AController* Exiting) override;

	/** 게임 시작시 호출됩니다. */
	UPROPERTY(BlueprintAssignable)
	FOnStartMatchSignature OnStartMatch;

protected:
	/** 플레이어 입장시 전송된 닉네임으로 플레이어 스테이트를 초기화해줍니다. */
	virtual FString InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal) override;

	virtual void HandleMatchHasStarted() override;

	virtual void HandleLeavingMap() override;

	/** 게임이 끝나면 다시 룸으로 이동할지 여부입니다. 테스트용으로 사용됩니다. */
	UPROPERTY(EditAnywhere, Category = "Design|Test")
	uint32 bReturnToRoomWhenGameEnds:1 = true;

	UPROPERTY()
	TWeakObjectPtr<ASMGameSession> RoomSession;
};
