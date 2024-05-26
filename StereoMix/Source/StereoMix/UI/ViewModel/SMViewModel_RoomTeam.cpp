// Copyright Surround, Inc. All Rights Reserved.


#include "SMViewModel_RoomTeam.h"

#include "StereoMix.h"
#include "Player/SMRoomPlayerState.h"
#include "Utilities/SMLog.h"

USMViewModel_RoomTeam::USMViewModel_RoomTeam()
{
	Team = ESMTeam::None;
}

void USMViewModel_RoomTeam::BindGameState(ASMRoomState* InRoomState)
{
	OwningRoomState = InRoomState;
	SetPlayer1(NewObject<USMViewModel_RoomPlayer>(GetWorld()));
	Players.Add(Player1);
	SetPlayer2(NewObject<USMViewModel_RoomPlayer>(GetWorld()));
	Players.Add(Player2);
	SetPlayer3(NewObject<USMViewModel_RoomPlayer>(GetWorld()));
	Players.Add(Player3);

	OwningRoomState->PlayerAddEvent.AddUObject(this, &USMViewModel_RoomTeam::OnPlayerAdd);
	OwningRoomState->PlayerRemoveEvent.AddUObject(this, &USMViewModel_RoomTeam::OnPlayerRemove);
	OwningRoomState->PlayerTeamChangeEvent.AddUObject(this, &USMViewModel_RoomTeam::OnPlayerTeamChanged);
}

void USMViewModel_RoomTeam::ChangeTeam()
{
	ASMRoomPlayerState* LocalPlayerState = GetWorld()->GetFirstPlayerController()->GetPlayerState<ASMRoomPlayerState>();
	if (!LocalPlayerState)
	{
		return;
	}

	LocalPlayerState->SetTeam(Team);
}
//
// void USMViewModel_RoomTeam::OnTeamPlayersUpdate(const ESMTeam TargetTeam)
// {
// 	if (TargetTeam != Team)
// 	{
// 		return;
// 	}
//
// 	const TArray<TObjectPtr<ASMRoomPlayerState>>& PlayerStates = OwningRoomState->GetTeamPlayers(Team);
//
// 	UE_LOG(LogStereoMix, Verbose, TEXT("[Room_Team_VM] [%s] Team players updated"), *UEnum::GetValueAsString(Team))
// 	for (int i = 0; i < Players.Num(); ++i)
// 	{
// 		if (i < PlayerStates.Num())
// 		{
// 			Players[i]->Init(PlayerStates[i]);
// 		}
// 		else
// 		{
// 			Players[i]->Remove();
// 		}
// 	}
// }

void USMViewModel_RoomTeam::OnPlayerAdd(APlayerState* NewPlayer)
{
	ASMRoomPlayerState* RoomPlayer = CastChecked<ASMRoomPlayerState>(NewPlayer);
	NET_LOG(RoomPlayer, Verbose, TEXT("[Room_Team_VM] [%s] Player %s added with team %s"), *UEnum::GetValueAsString(Team), *NewPlayer->GetPlayerName(), *UEnum::GetValueAsString(RoomPlayer->GetTeam()))
	if (RoomPlayer->GetTeam() != Team)
	{
		return;
	}

	const bool bAdded = AddPlayer(RoomPlayer);
	ensure(bAdded);

	SetTeamFull(OwningRoomState->GetPlayerCountInTeam(Team) >= OwningRoomState->MaxPlayersInTeam);
}

void USMViewModel_RoomTeam::OnPlayerRemove(APlayerState* RemovedPlayer)
{
	const ASMRoomPlayerState* RoomPlayer = CastChecked<ASMRoomPlayerState>(RemovedPlayer);
	NET_LOG(RoomPlayer, Verbose, TEXT("[Room_Team_VM] [%s] Player %s removed"), *UEnum::GetValueAsString(Team), *RemovedPlayer->GetPlayerName())
	if (RoomPlayer->GetTeam() != Team)
	{
		return;
	}

	const bool bRemoved = RemovePlayer(RoomPlayer);
	ensure(bRemoved);

	SetTeamFull(OwningRoomState->GetPlayerCountInTeam(Team) >= OwningRoomState->MaxPlayersInTeam);
}

void USMViewModel_RoomTeam::OnPlayerTeamChanged(APlayerState* TargetPlayer, const ESMTeam NewTeam)
{
	ASMRoomPlayerState* RoomPlayer = CastChecked<ASMRoomPlayerState>(TargetPlayer);

	if (NewTeam == ESMTeam::None)
	{
		return;
	}

	UE_LOG(LogStereoMix, Verbose, TEXT("[Room_Team_VM] [%s] Player %s changed team to %s"), *UEnum::GetValueAsString(Team), *TargetPlayer->GetPlayerName(), *UEnum::GetValueAsString(NewTeam))
	if (Team == NewTeam)
	{
		const bool bChanged = AddPlayer(RoomPlayer);
		ensure(bChanged);
	}
	else
	{
		RemovePlayer(RoomPlayer);
	}

	SetTeamFull(OwningRoomState->GetPlayerCountInTeam(Team) >= OwningRoomState->MaxPlayersInTeam);
}

bool USMViewModel_RoomTeam::AddPlayer(ASMRoomPlayerState* Player)
{
	bool bPlayerAdded = false;
	for (const TObjectPtr<USMViewModel_RoomPlayer>& PlayerViewModel : Players)
	{
		if (!PlayerViewModel->GetTargetPlayer())
		{
			PlayerViewModel->Init(Player);
			bPlayerAdded = true;

			if (PlayerViewModel->IsLocalPlayer())
			{
				SetIncludeLocalPlayer(true);
			}
			
			break;
		}
	}

	return bPlayerAdded;
}

bool USMViewModel_RoomTeam::RemovePlayer(const ASMRoomPlayerState* Player)
{
	bool bPlayerRemoved = false;
	for (int i = 0; i < Players.Num(); ++i)
	{
		auto TargetPlayerViewModel = Players[i];
		ensure(TargetPlayerViewModel);
		if (TargetPlayerViewModel->GetTargetPlayer() == Player)
		{
			if (TargetPlayerViewModel->IsLocalPlayer())
			{
				SetIncludeLocalPlayer(false);
			}
			TargetPlayerViewModel->Remove();
			bPlayerRemoved = true;
		}
		else if (bPlayerRemoved)
		{
			if (TargetPlayerViewModel->GetTargetPlayer() == nullptr)
			{
				break;
			}
			Players[i - 1]->Init(TargetPlayerViewModel->GetTargetPlayer());
			TargetPlayerViewModel->Remove();
		}
	}
	return bPlayerRemoved;
}
