// Copyright Surround, Inc. All Rights Reserved.


#include "SMViewModel_Room.h"

#include "Kismet/GameplayStatics.h"


USMViewModel_Room::USMViewModel_Room()
{
}

void USMViewModel_Room::BindGameState(ASMRoomState* InRoomState)
{
	ensure(InRoomState);

	OwningRoomState = InRoomState;

	SetTeamA(NewObject<USMViewModel_RoomTeam>(GetWorld()));
	TeamA->SetTeam(ESMTeam::EDM);
	TeamA->BindGameState(InRoomState);

	SetTeamB(NewObject<USMViewModel_RoomTeam>(GetWorld()));
	TeamB->SetTeam(ESMTeam::FutureBass);
	TeamB->BindGameState(InRoomState);
}

void USMViewModel_Room::QuitRoom()
{
	if (const UWorld* World = GetWorld())
	{
		World->GetGameInstance()->ReturnToMainMenu();
	}
}
