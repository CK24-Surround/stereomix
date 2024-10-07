// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "SMPlayerController.generated.h"

class USMPlayerControllerDataAsset;
class UInputMappingContext;
class USMControlData;
class ASMPlayerState;
class USMChatWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnChatReceived, const FString&, Name, const FString&, ChatMessage);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTeamChatReceived, const FString&, Name, const FString&, ChatMessage);

USTRUCT(BlueprintType)
struct FSMPlayerInputMappingContextInfo
{
	GENERATED_BODY()

	/** Target IMC */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<const UInputMappingContext> Context;

	/** IMC의 우선순위를 정합니다. 값이 높을 수록 우선순위가 높습니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	int32 Priority = 0;

	/** true인 경우 IMC를 Enhanced Input User Settings에 등록하거나 등록취소할 수 있습니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	uint8 bNotifyUserSettings:1;
};

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

	UFUNCTION(Reliable, Client)
	void ClientReceiveChat(const FString& Name, const FString& ChatMessage);

	UFUNCTION(Reliable, Client)
	void ClientReceiveTeamChat(const FString& Name, const FString& ChatMessage);

	const USMPlayerControllerDataAsset* GetDataAsset() const { return DataAsset; }

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

	UPROPERTY(EditAnywhere, Category = "Design")
	TObjectPtr<const USMPlayerControllerDataAsset> DataAsset;

private:
	void BroadcastChat(const FString& SenderName, const FString& ChatMessage, bool bTeamChat);

	UPROPERTY()
	TObjectPtr<const USMControlData> ControlData;

	UPROPERTY(EditDefaultsOnly, Category = "Input", meta = (AllowPrivateAccess))
	TArray<FSMPlayerInputMappingContextInfo> InputMappingContexts;
};
