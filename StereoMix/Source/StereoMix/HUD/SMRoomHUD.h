// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMBaseHUD.h"
#include "Games/SMRoomState.h"
#include "Player/SMRoomPlayerState.h"
#include "UI/ViewModel/SMViewModel_Room.h"
#include "SMRoomHUD.generated.h"

/**
 * Room HUD
 */
UCLASS()
class STEREOMIX_API ASMRoomHUD : public ASMBaseHUD
{
	GENERATED_BODY()

	UPROPERTY(Transient)
	TObjectPtr<ASMRoomState> RoomState;

	UPROPERTY(Transient)
	TObjectPtr<ASMRoomPlayerState> RoomPlayerState;
	
public:
	ASMRoomHUD();

	virtual void BeginPlay() override;

protected:
	UPROPERTY(Transient)
	TObjectPtr<USMViewModel_Room> RoomViewModel;
};
