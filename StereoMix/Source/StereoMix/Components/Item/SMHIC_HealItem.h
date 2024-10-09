// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMHIC_ItemBase.h"
#include "Actors/Tiles/SMTile.h"
#include "SMHIC_HealItem.generated.h"


class ASMHoldableItem_ChangeAttribute;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STEREOMIX_API USMHIC_HealItem : public USMHIC_ItemBase
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	virtual bool CanBeHeld(AActor* Instigator) const override;

	virtual void OnHeld(AActor* Instigator) override;

	virtual void OnReleasedFromHold(AActor* Instigator) override;

	virtual bool ShouldApplyDamageFromNoiseBreak(AActor* Instigator) const override { return false; }

	virtual bool ShouldCaptureTilesFromNoiseBreak(AActor* Instigator) const override { return false; }

	virtual void OnNoiseBreakApplied(ASMElectricGuitarCharacter* Instigator, TSharedPtr<FSMNoiseBreakData> NoiseBreakData) override;

	virtual void OnNoiseBreakApplied(ASMPianoCharacter* Instigator, TSharedPtr<FSMNoiseBreakData> NoiseBreakData) override;

	virtual void OnNoiseBreakApplied(ASMBassCharacter* Instigator, TSharedPtr<FSMNoiseBreakData> NoiseBreakData) override;

protected:
	void InternalNoiseBreakApplied(AActor* InActivator, const TArray<TWeakObjectPtr<ASMTile>>& TilesToBeCaptured);

	void OnHeldStateEntryCallback();

	UPROPERTY()
	TObjectPtr<ASMHoldableItem_ChangeAttribute> SourceItem;
};
