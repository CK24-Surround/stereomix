// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/HoldInteraction/SMHoldInteractionComponent.h"
#include "SMHIC_TrainingDummy.generated.h"


class ASMPlayerCharacterBase;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STEREOMIX_API USMHIC_TrainingDummy : public USMHoldInteractionComponent
{
	GENERATED_BODY()

public:
	USMHIC_TrainingDummy();

	virtual bool CanBeHeld(AActor* Instigator) const override;

	virtual void OnHeld(AActor* Instigator) override;

	virtual void OnReleasedFromHold(AActor* Instigator) override;

	virtual bool ShouldApplyDamageFromNoiseBreak() const override { return false; }

	virtual bool ShouldCaptureTilesFromNoiseBreak() const override { return true; }

	virtual void OnNoiseBreakStarted(AActor* Instigator) override;

	virtual void OnNoiseBreakApplied(ASMElectricGuitarCharacter* Instigator, TSharedPtr<FSMNoiseBreakData> NoiseBreakData) override;

	virtual void OnNoiseBreakApplied(ASMPianoCharacter* Instigator, TSharedPtr<FSMNoiseBreakData> NoiseBreakData) override;

	virtual void OnNoiseBreakApplied(ASMBassCharacter* Instigator, TSharedPtr<FSMNoiseBreakData> NoiseBreakData) override;

protected:
	void ReleasedFromHold();

	void InternalOnNoiseBreakApplied(ASMPlayerCharacterBase* Instigator, const TSharedPtr<FSMNoiseBreakData>& NoiseBreakData);
};
