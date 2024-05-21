// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UObject/Object.h"
#include "SMWidget_RoomId.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class STEREOMIX_API USMWidget_RoomId : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UTextBlock> RoomIdText;
};
