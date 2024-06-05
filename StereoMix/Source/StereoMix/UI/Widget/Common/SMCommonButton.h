// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "FMODEvent.h"
#include "SMCommonButton.generated.h"

/**
 * 
 */
UCLASS()
class STEREOMIX_API USMCommonButton : public UCommonButtonBase
{
	GENERATED_BODY()

protected:
	virtual void HandleFocusReceived() override;
	
	virtual void NativeOnHovered() override;

	virtual void NativeOnClicked() override;

private:
	// =============================================================================
	// Variables

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Sound", meta=(AllowPrivateAccess))
	TObjectPtr<UFMODEvent> HoverSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Sound", meta=(AllowPrivateAccess))
	TObjectPtr<UFMODEvent> ClickSound;
};
