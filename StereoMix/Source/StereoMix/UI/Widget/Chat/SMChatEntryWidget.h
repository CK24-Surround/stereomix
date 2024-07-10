// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "CommonUserWidget.h"

#include "SMChatEntryWidget.generated.h"

class UCommonTextStyle;
class UCommonTextBlock;

/**
 *
 */
UCLASS(Abstract)
class STEREOMIX_API USMChatEntryWidget : public UCommonUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

protected:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

private:
	// =============================================================================
	// Bindings

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Components", meta = (BindWidget, AllowPrivateAccess))
	TObjectPtr<UCommonTextBlock> NameTextBlock;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Components", meta = (BindWidget, AllowPrivateAccess))
	TObjectPtr<UCommonTextBlock> MessageTextBlock;


	// =============================================================================
	// Style

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Style", meta = (AllowPrivateAccess))
	TSubclassOf<UCommonTextStyle> AllChatTextStyle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Style", meta = (AllowPrivateAccess))
	TSubclassOf<UCommonTextStyle> TeamChatTextStyle;
};
