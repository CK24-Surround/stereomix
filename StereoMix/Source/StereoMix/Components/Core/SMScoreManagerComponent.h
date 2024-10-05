// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SMScoreManagerComponent.generated.h"


class ASMPlayerCharacterBase;

USTRUCT(BlueprintType)
struct FPlayerScoreData
{
	GENERATED_BODY()

	int32 TotalCapturedTiles = 0;

	int32 TotalDamageDealt = 0;

	int32 TotalNeutralizes = 0;

	int32 TotalNoiseBreakUsage = 0;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STEREOMIX_API USMScoreManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	UFUNCTION()
	void AddTotalCapturedTiles(const AActor* CapturedInstigator, int32 CapturedTilesCount);

	UFUNCTION()
	void AddTotalDamageDealt(const AActor* TargetPlayer, int32 DamageDealt);

	UFUNCTION()
	void AddTotalNeutralizes(const AActor* TargetPlayer, int32 Neutralizes);

	UFUNCTION()
	void AddTotalNoiseBreakUsage(const AActor* TargetPlayer, int32 NoiseBreakUsage);

protected:
	TMap<TObjectPtr<const AActor>, FPlayerScoreData> PlayerScoreData;
};
