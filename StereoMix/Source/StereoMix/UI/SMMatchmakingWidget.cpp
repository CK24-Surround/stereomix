// Fill out your copyright notice in the Description page of Project Settings.


#include "SMMatchmakingWidget.h"

#include "API/Matchmaking.h"
#include "Kismet/GameplayStatics.h"

void USMMatchmakingWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	OnRequestMatchComplete.AddDynamic(this, &USMMatchmakingWidget::HandleRequestMatchComplete);
	OnGetMatchStatusComplete.AddDynamic(this, &USMMatchmakingWidget::HandleGetMatchStatusComplete);
}

void USMMatchmakingWidget::NativeDestruct()
{
	Super::NativeDestruct();
	if (MatchStatusTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(MatchStatusTimerHandle);
	}
}

void USMMatchmakingWidget::StartMatchmaking(const FString& PlayerName)
{
	UMatchmaking* Matchmaking = GetGameInstance()->GetSubsystem<UMatchmaking>();
	const FString Mode = "test";
	UE_LOG(LogMatchmakingUI, Log, TEXT("RequestMatch: PlayerName=%s, Mode=%s"), *PlayerName, *Mode);
	Matchmaking->RequestMatch(PlayerName, Mode, OnRequestMatchComplete);
}

void USMMatchmakingWidget::HandleRequestMatchComplete_Implementation(const bool bSuccess,
                                                                     const FRequestMatchmakingResponse& Response)
{
	if (!bSuccess)
	{
		UE_LOG(LogMatchmakingUI, Error, TEXT("RequestMatch Failed"));
		return;
	}

	UE_LOG(LogMatchmakingUI, Log, TEXT("RequestMatch Complete: %s"), *Response.TicketId);
	MatchId = Response.TicketId;
	GetWorld()->GetTimerManager().SetTimer(MatchStatusTimerHandle, this,
	                                       &USMMatchmakingWidget::OnMatchStatusCheckTick, 5.0f, true);
}

void USMMatchmakingWidget::HandleGetMatchStatusComplete_Implementation(const bool bSuccess,
                                                                       const FGetMatchStatusResponse& Response)
{
	if (!bSuccess || Response.Result != EResponseResult::Success)
	{
		UE_LOG(LogMatchmakingUI, Error, TEXT("GetMatchStatus Failed"));
		GetWorld()->GetTimerManager().ClearTimer(MatchStatusTimerHandle);
		return;
	}

	UE_LOG(LogMatchmakingUI, Log, TEXT("MatchStatus: %s"), *UEnum::GetValueAsString(Response.MatchStatus.Status));

	switch (Response.MatchStatus.Status)
	{
	case EMatchStatusTypes::Succeeded:
	case EMatchStatusTypes::Failed:
	case EMatchStatusTypes::Cancelled:
	case EMatchStatusTypes::TimedOut:
		GetWorld()->GetTimerManager().ClearTimer(MatchStatusTimerHandle);
		break;
	default:
		break;
	}
}

void USMMatchmakingWidget::OnMatchStatusCheckTick() const
{
	UMatchmaking* Matchmaking = GetGameInstance()->GetSubsystem<UMatchmaking>();
	Matchmaking->GetMatchStatus(MatchId, OnGetMatchStatusComplete);
}

void USMMatchmakingWidget::ConnectToServer(const FString& IpAddress, int32 Port,
                                           const FString& PlayerSessionId, const FString& PlayerName)
{
	FString Uri = FString::Printf(TEXT("%s:%d?playerSessionId=%s&Nickname=%s"),
	                              *IpAddress, Port, *PlayerSessionId, *PlayerName);

	UE_LOG(LogMatchmakingUI, Log, TEXT("Start Game: %s"), *Uri);
	UGameplayStatics::OpenLevel(GetWorld(), *Uri);
}
