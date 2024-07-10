// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "SMPlayerController.generated.h"

class USMControlData;
class ASMPlayerState;
class USMChatWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnChatReceived, const FString&, Name, const FString&, ChatMessage);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTeamChatReceived, const FString&, Name, const FString&, ChatMessage);

/**
 * StereoMix Basic Player Controller
 */
UCLASS()
class STEREOMIX_API ASMPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ASMPlayerController();

	/**
	 * 전체 채팅 메시지를 서버로 전송합니다.
	 * @param ChatMessage 메시지
	 */
	UFUNCTION(Reliable, Server)
	void ServerSendChat(const FString& ChatMessage);

	/**
	 * 팀 채팅 메시지를 서버로 전송합니다.
	 * @param ChatMessage 메시지
	 */
	UFUNCTION(Reliable, Server)
	void ServerSendTeamChat(const FString& ChatMessage);

	FORCEINLINE const USMControlData* GetControlData() const { return ControlData; }

	/** 채팅 매시지를 받았을 때 호출되는 델리게이트입니다. */
	FOnChatReceived OnChatReceived;

	/** 팀 채팅 메시지를 받았을 때 호출되는 델리게이트입니다. */
	FOnTeamChatReceived OnTeamChatReceived;

protected:
	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;

	virtual void OnRep_PlayerState() override;

	void InitControl();

	UFUNCTION(Reliable, Client)
	void ClientReceiveChat(const FString& Name, const FString& ChatMessage);

	UFUNCTION(Reliable, Client)
	void ClientReceiveTeamChat(const FString& Name, const FString& ChatMessage);

private:
	void BroadcastChat(const FString& SenderName, const FString& ChatMessage, bool bTeamChat);

	UPROPERTY()
	TObjectPtr<const USMControlData> ControlData;
};
