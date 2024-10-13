#pragma once

namespace SMCollisionProfileName
{
	static FName NoCollision = TEXT("NoCollision");
	static FName BlockAll = TEXT("BlockAll");
	static FName InvisibleWallDynamic = TEXT("InvisibleWallDynamic");
	static FName Trigger = TEXT("Trigger");
	static FName Pawn = TEXT("Pawn");
	static FName Player = TEXT("Player");
	static FName PlayerProjectileHitbox = TEXT("PlayerProjectileHitbox");
	static FName PlayerTrigger = TEXT("PlayerTrigger");
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
	static FName SlashCollider = TEXT("SlashCollider");
}

namespace SMCollisionObjectChannel
{
	static ECollisionChannel Player = ECC_GameTraceChannel1;
	static ECollisionChannel Destructible = ECC_Destructible;
	static ECollisionChannel ActionOC = ECC_GameTraceChannel5;
}

namespace SMCollisionTraceChannel
{
	static ECollisionChannel ActionTC = ECC_GameTraceChannel2;
	static ECollisionChannel TileAction = ECC_GameTraceChannel4;
}
