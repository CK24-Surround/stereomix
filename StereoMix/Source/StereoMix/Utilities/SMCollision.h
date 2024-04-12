
#pragma once

namespace SMCollisionProfileName
{
	inline const FName NoCollision = TEXT("NoCollision");
	inline const FName Player = TEXT("Player");
	inline const FName Projectile = TEXT("Projectile");
}

namespace SMCollisionObjectChannel
{
	inline const ECollisionChannel Player = ECC_GameTraceChannel1;
}

namespace SMCollisionTraceChannel
{
	inline const ECollisionChannel Action = ECC_GameTraceChannel2;
}
