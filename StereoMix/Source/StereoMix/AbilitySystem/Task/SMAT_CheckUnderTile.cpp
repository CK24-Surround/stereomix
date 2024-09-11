// Copyright Surround, Inc. All Rights Reserved.


#include "SMAT_CheckUnderTile.h"

#include "Characters/Player/SMPlayerCharacterBase.h"
#include "Tiles/SMTile.h"
#include "Utilities/SMCollision.h"


USMAT_CheckUnderTile::USMAT_CheckUnderTile()
{
	bTickingTask = true;
}

USMAT_CheckUnderTile* USMAT_CheckUnderTile::CheckUnderTile(UGameplayAbility* OwningAbility, ASMPlayerCharacterBase* NewSourceCharacter)
{
	USMAT_CheckUnderTile* NewObj = NewAbilityTask<USMAT_CheckUnderTile>(OwningAbility);
	NewObj->SourceCharacter = NewSourceCharacter;
	return NewObj;
}

void USMAT_CheckUnderTile::TickTask(float DeltaTime)
{
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

		if (Tile == LastCheckedTile)
		{
			return;
		}

		LastCheckedTile = Tile;

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
