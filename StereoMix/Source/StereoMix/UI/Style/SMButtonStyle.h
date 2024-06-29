// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "FMODEvent.h"

#include "SMButtonStyle.generated.h"

/**
 *
 */
UCLASS()
class STEREOMIX_API USMButtonStyle : public UCommonButtonStyle
{
	GENERATED_BODY()

public:
	// =============================================================================
	// Variables

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound", meta = (AllowPrivateAccess), DisplayName = "Hover Sound (FMOD)")
	TObjectPtr<UFMODEvent> HoverSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound", meta = (AllowPrivateAccess), DisplayName = "Click Sound (FMOD)")
	TObjectPtr<UFMODEvent> ClickSound;
};
