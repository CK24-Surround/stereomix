﻿// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMChatWidget.h"
#include "UI/Widget/Common/SMButtonBase.h"
#include "SMChatTypeButton.generated.h"

class UCommonTextBlock;
/**
 * 
 */
UCLASS()
class STEREOMIX_API USMChatTypeButton : public USMButtonBase
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void SetChatTypeText(EChatType ChatType);

private:
	// =============================================================================
	// Bindings

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Components", meta = (BindWidget, AllowPrivateAccess))
	TObjectPtr<UCommonTextBlock> ChatTypeTextBlock;
};
