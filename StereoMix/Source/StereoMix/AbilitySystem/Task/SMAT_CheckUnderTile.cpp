// Copyright Surround, Inc. All Rights Reserved.


#include "SMAT_CheckUnderTile.h"

#include "Actors/Character/Player/SMPlayerCharacterBase.h"
#include "Actors/Tiles/SMTile.h"
#include "Utilities/SMCollision.h"
#include "Utilities/SMLog.h"


USMAT_CheckUnderTile::USMAT_CheckUnderTile()
{
	bTickingTask = true;
}

USMAT_CheckUnderTile* USMAT_CheckUnderTile::CheckUnderTile(UGameplayAbility* OwningAbility, ASMPlayerCharacterBase* NewSourceCharacter, float TickRate)
{
	USMAT_CheckUnderTile* NewObj = NewAbilityTask<USMAT_CheckUnderTile>(OwningAbility);
	NewObj->SourceCharacter = NewSourceCharacter;
	NewObj->TickInterval = 1.0f / TickRate;
	return NewObj;
}

void USMAT_CheckUnderTile::TickTask(float DeltaTime)
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
	const FVector StartLocation = SourceCharacter->GetActorLocation();
	const FVector EndLocation = StartLocation + (FVector::DownVector * 1000.0f);
	if (GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, SMCollisionTraceChannel::TileAction))
	{
		ASMTile* Tile = Cast<ASMTile>(HitResult.GetActor());
		if (!Tile)
		{
			return;
		}

		if (ShouldBroadcastAbilityTaskDelegates())
		{
			(void)OnUnderTileChanged.ExecuteIfBound(Tile);
		}
	}
}

void USMAT_CheckUnderTile::OnDestroy(bool bInOwnerFinished)
{
	OnUnderTileChanged.Unbind();

	Super::OnDestroy(bInOwnerFinished);
}
