﻿// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonTextBlock.h"
#include "UI/Widget/Common/SMButtonBase.h"

#include "SMRoomCodeCopyButton.generated.h"

/**
 *
 */
UCLASS()
class STEREOMIX_API USMRoomCodeCopyButton : public USMButtonBase
{
	GENERATED_BODY()

public:
	void SetRoomCode(const FString& RoomCode);

protected:
	virtual void NativeOnClicked() override;

private:
	// =============================================================================
	// Bindings

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Components", meta = (BindWidget, AllowPrivateAccess))
	TObjectPtr<UCommonTextBlock> RoomCodeTextBlock;
};
