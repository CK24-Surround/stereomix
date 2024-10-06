// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMFunctionLibraryBase.h"
#include "Data/SMTeam.h"
#include "SMProjectileFunctionLibrary.generated.h"

class ASMProjectile;
class USMProjectilePoolManagerComponent;
/**
 * 
 */
UCLASS()
class STEREOMIX_API USMProjectileFunctionLibrary : public USMFunctionLibraryBase
{
	GENERATED_BODY()

public:
	static USMProjectilePoolManagerComponent* GetPoolManager(const UWorld* World);

	static ASMProjectile* GetElectricGuitarProjectile(const UWorld* World, ESMTeam Team);
	
	static ASMProjectile* GetPianoProjectile(const UWorld* World, ESMTeam Team, int32 ChargeLevel);
	
	static ASMProjectile* GetSlowBulletProjectile(const UWorld* World, ESMTeam Team);
};
