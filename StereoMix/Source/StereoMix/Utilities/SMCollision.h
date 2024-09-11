#pragma once

namespace SMCollisionProfileName
{
	static FName NoCollision = TEXT("NoCollision");
	static FName Trigger = TEXT("Trigger");
	static FName Player = TEXT("Player");
	static FName Projectile = TEXT("Projectile");
	static FName Tile = TEXT("Tile");
	static FName HealPack = TEXT("HealPack");
	static FName Gimmick = TEXT("Gimmick");
	static FName CatchableItem = TEXT("CatchableItem");
	static FName Ghost = TEXT("Ghost");
	static FName NoiseBreak = TEXT("NoiseBreak");
}

namespace SMCollisionObjectChannel
{
	static ECollisionChannel Player = ECC_GameTraceChannel1;
}

namespace SMCollisionTraceChannel
{
	static ECollisionChannel Action = ECC_GameTraceChannel2;
	static ECollisionChannel TileAction = ECC_GameTraceChannel4;
}
