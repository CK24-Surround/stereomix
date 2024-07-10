// Copyright Surround, Inc. All Rights Reserved.


#include "SMPlayerController.h"

#include "GameFramework/GameModeBase.h"
#include "GameFramework/GameStateBase.h"
#include "EnhancedInputSubsystems.h"
#include "Data/SMControlData.h"
#include "Games/SMPlayerState.h"
#include "HUD/SMClientHUD.h"
#include "Utilities/SMAssetPath.h"

ASMPlayerController::ASMPlayerController()
{
	static ConstructorHelpers::FObjectFinder<USMControlData> ControlDataRef(SMAssetPath::ControlData);
	if (ControlDataRef.Object)
	{
		ControlData = ControlDataRef.Object;
	}
	else
	{
		UE_LOG(LogLoad, Error, TEXT("ControlData 로드에 실패했습니다."));
	}
}

void ASMPlayerController::ServerSendChat_Implementation(const FString& ChatMessage)
{
	const FString SenderName = PlayerState->GetPlayerName();
	BroadcastChat(SenderName, ChatMessage, false);
}

void ASMPlayerController::ServerSendTeamChat_Implementation(const FString& ChatMessage)
{
	const FString SenderName = PlayerState->GetPlayerName();
	BroadcastChat(SenderName, ChatMessage, true);
}

void ASMPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void ASMPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InitControl();
}

void ASMPlayerController::InitControl()
{
	if (!IsLocalController())
	{
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (Subsystem)
	{
		Subsystem->ClearAllMappings();
		Subsystem->AddMappingContext(GetControlData()->DefaultMappingContext, 0);
		Subsystem->AddMappingContext(GetControlData()->UIMappingContext, 0);
	}
}

void ASMPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (PlayerState != nullptr)
	{
		if (AHUD* HUD = GetHUD())
		{
			if (ASMClientHUD* ClientHUD = Cast<ASMClientHUD>(HUD))
			{
				ClientHUD->InitPlayerState(PlayerState);
			}
		}
	}
}

void ASMPlayerController::ClientReceiveChat_Implementation(const FString& Name, const FString& ChatMessage)
{
	if (OnChatReceived.IsBound())
	{
		OnChatReceived.Broadcast(Name, ChatMessage);
	}
}

void ASMPlayerController::ClientReceiveTeamChat_Implementation(const FString& Name, const FString& ChatMessage)
{
	if (OnTeamChatReceived.IsBound())
	{
		OnTeamChatReceived.Broadcast(Name, ChatMessage);
	}
}

void ASMPlayerController::BroadcastChat(const FString& SenderName, const FString& ChatMessage, bool bTeamChat)
{
	const ASMPlayerState* SMPlayerState = GetPlayerState<ASMPlayerState>();
	if (SMPlayerState == nullptr)
	{
		return;
	}

	const AGameModeBase* GameMode = GetWorld()->GetAuthGameMode();
	if (GameMode == nullptr)
	{
		return;
	}

	const ESMTeam Team = SMPlayerState->GetTeam();
	if (Team == ESMTeam::None && bTeamChat)
	{
		return;
	}

	for (const TObjectPtr<APlayerState>& TargetPlayer : GameMode->GameState->PlayerArray)
	{
		ASMPlayerState* TargetPlayerState = CastChecked<ASMPlayerState>(TargetPlayer);
		if (bTeamChat)
		{
			if (TargetPlayerState->GetTeam() == Team)
			{
				ASMPlayerController* TargetController = CastChecked<ASMPlayerController>(TargetPlayerState->GetOwningController());
				TargetController->ClientReceiveTeamChat(SenderName, ChatMessage);
			}
		}
		else
		{
			ASMPlayerController* TargetController = CastChecked<ASMPlayerController>(TargetPlayerState->GetOwningController());
			TargetController->ClientReceiveChat(SenderName, ChatMessage);
		}
	}
}
