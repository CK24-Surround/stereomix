// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMFrontendPlayerController.h"
#include "SMPlayerState.h"
#include "SMRoomPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class STEREOMIX_API ASMRoomPlayerController : public ASMFrontendPlayerController
{
	GENERATED_BODY()

	TWeakObjectPtr<ASMPlayerState> RoomPlayerState;

public:
	UPROPERTY(BlueprintAssignable)
	FTeamChangedEvent TeamChangedEvent;

	ASMRoomPlayerController();

	virtual void OnRep_PlayerState() override;

	virtual void BeginPlay() override;

	UFUNCTION(Reliable, Server)
	void ChangeTeam(ESMTeam NewTeam);

protected:
	UFUNCTION()
	void OnTeamChanged(ESMTeam NewTeam);
};
