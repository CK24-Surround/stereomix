// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Player/SMPlayerState.h"
#include "SMRoomPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class STEREOMIX_API ASMRoomPlayerState : public ASMPlayerState
{
	GENERATED_BODY()

public:
	ASMRoomPlayerState();

	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	virtual bool CanChangeTeam(ESMTeam NewTeam) const override;
	virtual void OnTeamChanged(ESMTeam PreviousTeam, ESMTeam NewTeam) override;
};
