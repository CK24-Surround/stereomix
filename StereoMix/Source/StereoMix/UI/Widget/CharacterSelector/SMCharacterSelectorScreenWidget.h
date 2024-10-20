// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "SMCharacterSelectorScreenWidget.generated.h"

class UCommonTextBlock;
class UButton;
/**
 * 
 */
UCLASS(Abstract)
class STEREOMIX_API USMCharacterSelectorScreenWidget : public UCommonUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SelectButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> SelectButtonText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> PickElectricGuitar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> PickPiano;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> PickBass;
};
