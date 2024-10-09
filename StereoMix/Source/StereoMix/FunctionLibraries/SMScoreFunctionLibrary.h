// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMFunctionLibraryBase.h"
#include "SMScoreFunctionLibrary.generated.h"

class USMScoreManagerComponent;
/**
 * 
 */
UCLASS()
class STEREOMIX_API USMScoreFunctionLibrary : public USMFunctionLibraryBase
{
	GENERATED_BODY()

public:
	static USMScoreManagerComponent* GetScoreManager(const UWorld* World);

	static void RecordDamage(AActor* Self, const AActor* Attacker, float InDamageAmount);

	static void RecordNoiseBreakUsage(const AActor* Self);

	static void RecordKillDeathCount(const AActor* LastAttacker, const AActor* Self);
};
