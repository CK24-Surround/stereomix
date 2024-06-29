// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "SMGameStateNotify.generated.h"

enum class ESMTeam : uint8;
enum class ESMCharacterType : uint8;
class ASMPlayerState;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerJoined, ASMPlayerState*, JoinedPlayer);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerLeft, ASMPlayerState*, LeftPlayer);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FPlayerTeamChanged, ASMPlayerState*, Player, ESMTeam, PreviousTeam, ESMTeam, NewTeam);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPlayerCharacterChanged, ASMPlayerState*, Player, ESMCharacterType, NewCharacter);

// This class does not need to be modified.
UINTERFACE()
class USMGameStateNotify : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class STEREOMIX_API ISMGameStateNotify
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void NotifyPlayerJoined(ASMPlayerState* JoinedPlayer) = 0;
	virtual void NotifyPlayerLeft(ASMPlayerState* LeftPlayer) = 0;
	virtual void NotifyPlayerTeamChanged(ASMPlayerState* Player, ESMTeam PreviousTeam, ESMTeam NewTeam) = 0;
	virtual void NotifyPlayerCharacterChanged(ASMPlayerState* Player, ESMCharacterType NewCharacter) = 0;
};
