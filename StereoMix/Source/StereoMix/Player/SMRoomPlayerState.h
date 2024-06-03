// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Games/SMRoomState.h"
#include "Player/SMPlayerState.h"
#include "SMRoomPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class STEREOMIX_API ASMRoomPlayerState : public ASMPlayerState
{
	GENERATED_BODY()

	TWeakObjectPtr<ASMRoomState> RoomState;
	
public:
	ASMRoomPlayerState();

	virtual void BeginPlay() override;
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void ChangeTeam(ESMTeam NewTeam);

protected:
	virtual bool CanChangeTeam(ESMTeam NewTeam) const override;
	
	virtual void OnTeamChanged(ESMTeam PreviousTeam, ESMTeam NewTeam) override;

	virtual bool CanChangeCharacterType(ESMCharacterType NewCharacterType) const override;
};
