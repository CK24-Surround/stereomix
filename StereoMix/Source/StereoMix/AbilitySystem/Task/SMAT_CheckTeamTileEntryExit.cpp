// Copyright Surround, Inc. All Rights Reserved.


#include "SMAT_CheckTeamTileEntryExit.h"

#include "Actors/Character/Player/SMPlayerCharacterBase.h"
#include "Actors/Tiles/SMTile.h"
#include "Utilities/SMCollision.h"
#include "Utilities/SMLog.h"

USMAT_CheckTeamTileEntryExit::USMAT_CheckTeamTileEntryExit()
{
	bTickingTask = true;
}

USMAT_CheckTeamTileEntryExit* USMAT_CheckTeamTileEntryExit::CheckTeamTileEntryExit(UGameplayAbility* OwningAbility, ASMPlayerCharacterBase* NewSourceCharacter)
{
	USMAT_CheckTeamTileEntryExit* MyObj = NewAbilityTask<USMAT_CheckTeamTileEntryExit>(OwningAbility);
	MyObj->SourceCharacter = NewSourceCharacter;
	return MyObj;
}

void USMAT_CheckTeamTileEntryExit::TickTask(float DeltaTime)
{
	AccumulatedTime += DeltaTime;
	if (AccumulatedTime < TickInterval)
	{
		return;
	}

	AccumulatedTime -= TickInterval;

	if (!SourceCharacter.Get())
	{
		return;
	}

	FHitResult HitResult;
	const FVector Start = SourceCharacter->GetActorLocation();
	const FVector End = Start + (FVector::DownVector * 1000.0f);
	ECollisionChannel Channel = SMCollisionTraceChannel::TileAction;
	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, Channel))
	{
		ASMTile* Tile = Cast<ASMTile>(HitResult.GetActor());
		if (Tile)
		{
			ESMTeam TileTeam = Tile->GetTeam();
			ESMTeam SourceTeam = SourceCharacter->GetTeam();
			if (SourceTeam == TileTeam)
			{
				SetIsEntry(true);
			}
			else
			{
				SetIsEntry(false);
			}
		}
	}
}

void USMAT_CheckTeamTileEntryExit::OnDestroy(bool bInOwnerFinished)
{
	OnTeamTileEntry.Unbind();
	OnTeamTileExit.Unbind();

	Super::OnDestroy(bInOwnerFinished);
}

void USMAT_CheckTeamTileEntryExit::SetIsEntry(bool bNewIsEntry)
{
	if (bIsEntry != bNewIsEntry)
	{
		bIsEntry = bNewIsEntry;
		if (bIsEntry)
		{
			OnTeamTileEntryCallback();
		}
		else
		{
			OnTeamTileExitCallback();
		}
	}
}

void USMAT_CheckTeamTileEntryExit::OnTeamTileEntryCallback()
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		(void)OnTeamTileEntry.ExecuteIfBound();
	}
}

void USMAT_CheckTeamTileEntryExit::OnTeamTileExitCallback()
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		(void)OnTeamTileExit.ExecuteIfBound();
	}
}
