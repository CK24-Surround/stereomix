// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "FMODEvent.h"

#include "SMButtonBase.generated.h"

/**
 *
 */
UCLASS()
class STEREOMIX_API USMButtonBase : public UCommonButtonBase
{
	GENERATED_BODY()

protected:
	virtual void NativeOnHovered() override;

	virtual void NativeOnSelected(bool bBroadcast) override;

	virtual void NativeOnPressed() override;

private:
	// =============================================================================
	// Variables

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound", meta = (AllowPrivateAccess), DisplayName = "Hovered Sound Override (FMOD)")
	TObjectPtr<UFMODEvent> HoveredFMODSoundOverride;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound", meta = (AllowPrivateAccess), DisplayName = "Selected Sound Override (FMOD)")
	TObjectPtr<UFMODEvent> SelectedFMODSoundOverride;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound", meta = (AllowPrivateAccess), DisplayName = "Pressed Sound Override (FMOD)")
	TObjectPtr<UFMODEvent> PressedFMODSoundOverride;
};
