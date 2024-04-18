// Fill out your copyright notice in the Description page of Project Settings.


#include "SMGameMode.h"

#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"

FString ASMGameMode::InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal)
{
	const FString ErrorMessage = Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);

	FString InNickname = UGameplayStatics::ParseOption(Options, TEXT("Nickname")).Left(20);
	if (InNickname.IsEmpty())
	{
		InNickname = FString::Printf(TEXT("%s%i"), *DefaultPlayerName.ToString(), NewPlayerController->PlayerState->GetPlayerId());
	}

	ChangeName(NewPlayerController, InNickname, false);

	return ErrorMessage;
}
