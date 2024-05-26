// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/SMCharacterType.h"
#include "Data/SMTeam.h"
#include "GameFramework/PlayerState.h"
#include "SMPlayerState.generated.h"

DECLARE_EVENT_OneParam(ASMRoomPlayerState, FTeamChangedEvent, ESMTeam /*ChangedTeam*/);
DECLARE_EVENT_OneParam(ASMRoomPlayerState, FCharacterTypeChangedEvent, ESMCharacterType /*ChangedCharacterType*/);

/**
 * 
 */
UCLASS()
class STEREOMIX_API ASMPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void SeamlessTravelTo(APlayerState* NewPlayerState) override;

public:
	FTeamChangedEvent TeamChangedEvent;
	FCharacterTypeChangedEvent CharacterTypeChangedEvent;
	
	FORCEINLINE ESMTeam GetTeam() const { return Team; }

	UFUNCTION(Reliable, Server) 
	void SetTeam(ESMTeam NewTeam);

	FORCEINLINE ESMCharacterType GetCharacterType() { return CharacterType; }

	UFUNCTION(Reliable, Server)
	void SetCharacterType(ESMCharacterType NewCharacterType);

	virtual void OnTeamChanged(ESMTeam PreviousTeam, ESMTeam NewTeam);

	virtual void OnCharacterTypeChanged(ESMCharacterType PreviousCharacterType, ESMCharacterType NewCharacterType);

protected:
	UPROPERTY(VisibleAnywhere, ReplicatedUsing= OnRep_Team)
	ESMTeam Team;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing= OnRep_CharacterType)
	ESMCharacterType CharacterType;

	UFUNCTION()
	virtual void OnRep_Team(ESMTeam PreviousTeam);

	UFUNCTION()
	virtual void OnRep_CharacterType(ESMCharacterType PreviousCharacterType);

	virtual bool CanChangeTeam(ESMTeam NewTeam) const;

	virtual bool CanChangeCharacterType(ESMCharacterType NewCharacterType) const;
};
