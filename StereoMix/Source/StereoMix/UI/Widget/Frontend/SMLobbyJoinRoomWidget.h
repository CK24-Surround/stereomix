// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMLobbyProcessWidget.h"
#include "SMLobbyJoinRoomWidget.generated.h"

/**
 * StereoMix Lobby Join Room Widget
 */
UCLASS()
class STEREOMIX_API USMLobbyJoinRoomWidget : public USMLobbyProcessWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadWrite, Category = "Lobby")
	FString RoomCode;
	
	USMLobbyJoinRoomWidget();

protected:
	virtual void NativeConstruct() override;
	
	virtual void NativeOnActivated() override;

	virtual void NativeOnDeactivated() override;

	UFUNCTION()
	virtual void OnJoinRoomWithCodeResponse(EJoinRoomWithCodeResult Result, const FString& ServerUrl);
};
