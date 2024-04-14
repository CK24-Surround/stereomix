
#pragma once

namespace SMCollisionProfileName
{
	static FName NoCollision = TEXT("NoCollision");
	static FName Player = TEXT("Player");
	static FName Projectile = TEXT("Projectile");
}

namespace SMCollisionObjectChannel
{
	static ECollisionChannel Player = ECC_GameTraceChannel1;
}

namespace SMCollisionTraceChannel
{
	static ECollisionChannel Action = ECC_GameTraceChannel2;
}
