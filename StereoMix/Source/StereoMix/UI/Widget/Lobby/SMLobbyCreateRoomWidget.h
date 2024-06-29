// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMLobbyProcessWidget.h"

#include "SMLobbyCreateRoomWidget.generated.h"

/**
 * StereoMix Lobby Create Room Widget
 */
UCLASS(Abstract)
class STEREOMIX_API USMLobbyCreateRoomWidget : public USMLobbyProcessWidget
{
	GENERATED_BODY()

public:
	USMLobbyCreateRoomWidget();

protected:
	virtual void NativeOnActivated() override;

	virtual void NativeOnDeactivated() override;

	UFUNCTION()
	virtual void OnCreateRoomResponse(ECreateRoomResult Result, const FString& ServerUrl);
};
