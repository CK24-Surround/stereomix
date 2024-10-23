// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "SMTutorialKeyInfo.generated.h"

class UOverlay;
/**
 * 
 */
UCLASS()
class STEREOMIX_API USMTutorialKeyInfo : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	USMTutorialKeyInfo();

	virtual void NativeConstruct() override;

	void HideAll();

	UFUNCTION(BlueprintNativeEvent, Category = "KeyInfo")
	bool ShowLeftClick();

	UFUNCTION(BlueprintNativeEvent, Category = "KeyInfo")
	bool ShowRightClick();

	UFUNCTION(BlueprintNativeEvent, Category = "KeyInfo")
	bool ShowRightLeftClick();

	UFUNCTION(BlueprintNativeEvent, Category = "KeyInfo")
	bool ShowWASDKey();

	UFUNCTION(BlueprintNativeEvent, Category = "KeyInfo")
	bool ShowEKey();

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> LeftClickOverlay;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> RightClickOverlay;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> RightLeftClickOverlay;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> WASDOverlay;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> EOverlay;

protected:
	UPROPERTY()
	TArray<TObjectPtr<UWidget>> Overlays;
};
