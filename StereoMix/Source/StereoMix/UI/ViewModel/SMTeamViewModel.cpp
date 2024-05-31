// Copyright Surround, Inc. All Rights Reserved.


#include "SMTeamViewModel.h"

#include "StereoMix.h"
#include "Player/SMRoomPlayerState.h"
#include "Utilities/SMLog.h"

USMTeamViewModel::USMTeamViewModel()
{
	Team = ESMTeam::None;
}

void USMTeamViewModel::BindGameState(ASMRoomState* InRoomState)
{
	OwningRoomState = InRoomState;
	SetPlayer1(NewObject<USMRoomPlayerViewModel>(GetWorld()));
	Players.Add(Player1);
	SetPlayer2(NewObject<USMRoomPlayerViewModel>(GetWorld()));
	Players.Add(Player2);
	SetPlayer3(NewObject<USMRoomPlayerViewModel>(GetWorld()));
	Players.Add(Player3);

	for (auto PlayerViewModel : Players)
	{
		PlayerViewModel->Remove();
	}

	OwningRoomState->OnTeamPlayersUpdated.AddDynamic(this, &USMTeamViewModel::OnTeamPlayersUpdated);
}

void USMTeamViewModel::ChangeTeam()
{
	ASMRoomPlayerState* LocalPlayerState = GetWorld()->GetFirstPlayerController()->GetPlayerState<ASMRoomPlayerState>();
	if (!LocalPlayerState)
	{
		return;
	}

	NET_LOG(LocalPlayerState, Verbose, TEXT("[Room_Team_VM] [%s] Player %s changing team"), *UEnum::GetValueAsString(Team), *LocalPlayerState->GetPlayerName())
	LocalPlayerState->SetTeam(Team);
}

void USMTeamViewModel::OnTeamPlayersUpdated(const ESMTeam UpdatedTeam)
{
	if (UpdatedTeam != Team)
	{
		return;
	}

	const TArray<ASMRoomPlayerState*>& TeamPlayers = OwningRoomState->GetTeamPlayers(Team);
	for (int i = 0; i < Players.Num(); ++i)
	{
		if (i < TeamPlayers.Num())
		{
			Players[i]->Init(TeamPlayers[i]);
		}
		else
		{
			Players[i]->Remove();
		}
	}
}

// void USMViewModel_RoomTeam::OnPlayerAdd(APlayerState* NewPlayer)
// {
// 	ASMRoomPlayerState* RoomPlayer = CastChecked<ASMRoomPlayerState>(NewPlayer);
// 	// NET_LOG(RoomPlayer, Verbose, TEXT("[Room_Team_VM] [%s] Player %s added with team %s"), *UEnum::GetValueAsString(Team), *NewPlayer->GetPlayerName(), *UEnum::GetValueAsString(RoomPlayer->GetTeam()))
// 	if (RoomPlayer->GetTeam() != Team)
// 	{
// 		return;
// 	}
//
// 	const bool bAdded = AddPlayer(RoomPlayer);
// 	ensure(bAdded);
//
// 	SetTeamFull(OwningRoomState->GetPlayerCountInTeam(Team) >= OwningRoomState->MaxPlayersInTeam);
// }
//
// void USMViewModel_RoomTeam::OnPlayerRemove(APlayerState* RemovedPlayer)
// {
// 	if (!OwningRoomState.IsValid())
// 	{
// 		return;
// 	}
// 	
// 	const ASMRoomPlayerState* RoomPlayer = CastChecked<ASMRoomPlayerState>(RemovedPlayer);
// 	// NET_LOG(RoomPlayer, Verbose, TEXT("[Room_Team_VM] [%s] Player %s removed"), *UEnum::GetValueAsString(Team), *RemovedPlayer->GetPlayerName())
// 	if (RoomPlayer->GetTeam() != Team)
// 	{
// 		return;
// 	}
//
// 	const bool bRemoved = RemovePlayer(RoomPlayer);
// 	ensure(bRemoved);
//
// 	SetTeamFull(OwningRoomState->GetPlayerCountInTeam(Team) >= OwningRoomState->MaxPlayersInTeam);
// }
//
// void USMViewModel_RoomTeam::OnPlayerTeamChanged(APlayerState* TargetPlayer, const ESMTeam NewTeam)
// {
// 	ASMRoomPlayerState* RoomPlayer = CastChecked<ASMRoomPlayerState>(TargetPlayer);
//
// 	if (NewTeam == ESMTeam::None)
// 	{
// 		return;
// 	}
//
// 	// UE_LOG(LogStereoMix, Verbose, TEXT("[Room_Team_VM] [%s] Player %s changed team to %s"), *UEnum::GetValueAsString(Team), *TargetPlayer->GetPlayerName(), *UEnum::GetValueAsString(NewTeam))
// 	if (Team == NewTeam)
// 	{
// 		const bool bChanged = AddPlayer(RoomPlayer);
// 		ensure(bChanged);
// 	}
// 	else
// 	{
// 		RemovePlayer(RoomPlayer);
// 	}
//
// 	SetTeamFull(OwningRoomState->GetPlayerCountInTeam(Team) >= OwningRoomState->MaxPlayersInTeam);
// }

bool USMTeamViewModel::AddPlayer(ASMRoomPlayerState* Player)
{
	bool bPlayerAdded = false;
	for (const TObjectPtr<USMRoomPlayerViewModel>& PlayerViewModel : Players)
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

bool USMTeamViewModel::RemovePlayer(const ASMRoomPlayerState* Player)
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
