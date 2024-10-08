// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "SMHoldableItemBase.h"
#include "SMHoldableItem_ChangeAttribute.generated.h"

class UGameplayEffect;

UCLASS()
class STEREOMIX_API ASMHoldableItem_ChangeAttribute : public ASMHoldableItemBase
{
	GENERATED_BODY()

public:
	ASMHoldableItem_ChangeAttribute(const FObjectInitializer& ObjectInitializer);

	virtual void ActivateItemByNoiseBreak(const UWorld* World, const TArray<ASMTile*>& TilesToBeCaptured, AActor* InActivator, const TOptional<ESMTeam>& TeamOption) override;

	virtual void OnHeldStateEntry() override;

	virtual void OnHeldStateExit() override;

	virtual void BeginDestroy() override;

protected:
	void TriggerCountTimerCallback();

	UPROPERTY(EditAnywhere, Category = "Design")
	TSubclassOf<UGameplayEffect> SelfGE;

	UPROPERTY(EditAnywhere, Category = "Design")
	FGameplayTag SelfDataTag;

	UPROPERTY(EditAnywhere, Category = "Design")
	float SelfMagnitude = 10.0f;

	UPROPERTY(EditAnywhere, Category = "Design")
	TSubclassOf<UGameplayEffect> GE;

	UPROPERTY(EditAnywhere, Category = "Design")
	FGameplayTag DataTag;

	UPROPERTY(EditAnywhere, Category = "Design")
	float TotalMagnitude = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Design", meta = (ClampMin = "0.0"))
	float Duration = 5.0f;

	UPROPERTY(EditAnywhere, Category = "Design", meta = (ClampMin = "1"))
	int32 TriggerCount = 1;

	TArray<TObjectPtr<ASMTile>> CachedTiles = TArray<TObjectPtr<ASMTile>>();

	FTimerHandle TriggerCountTimerHandle;

	int32 CurrentTriggerCount = 0;

	int8 bActivated:1 = false;
};
