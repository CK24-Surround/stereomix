#pragma once

namespace SMCollisionProfileName
{
	static FName NoCollision = TEXT("NoCollision");
	static FName BlockAll = TEXT("BlockAll");
	static FName Trigger = TEXT("Trigger");
	static FName Pawn = TEXT("Pawn");
	static FName Player = TEXT("Player");
	static FName Projectile = TEXT("Projectile");
	static FName Tile = TEXT("Tile");
	static FName HealPack = TEXT("HealPack");
	static FName Gimmick = TEXT("Gimmick");
	static FName Obstacle = TEXT("Obstacle");
	static FName HoldableItem = TEXT("HoldableItem");
	static FName OverlapItem = TEXT("OverlapItem");
	static FName CatchableItem = TEXT("CatchableItem");
	static FName Charge = TEXT("Charge");
	static FName NoiseBreak = TEXT("NoiseBreak");
	static FName Ghost = TEXT("Ghost");
}

namespace SMCollisionObjectChannel
{
	static ECollisionChannel Player = ECC_GameTraceChannel1;
	static ECollisionChannel Obstacle = ECC_GameTraceChannel8;
}

namespace SMCollisionTraceChannel
{
	static ECollisionChannel Action = ECC_GameTraceChannel2;
	static ECollisionChannel TileAction = ECC_GameTraceChannel4;
}
