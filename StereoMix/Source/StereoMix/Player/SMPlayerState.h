// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/SMCharacterType.h"
#include "Data/SMTeam.h"
#include "GameFramework/PlayerState.h"
#include "SMPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class STEREOMIX_API ASMPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	FORCEINLINE ESMTeam GetTeam() { return Team; }

	FORCEINLINE void SetTeam(ESMTeam NewTeam) { Team = NewTeam; }

	FORCEINLINE ESMCharacterType GetCharacterType() { return CharacterType; }

	FORCEINLINE void SetCharacterType(ESMCharacterType NewCharacterType) { CharacterType = NewCharacterType; }

protected:
	UPROPERTY(Replicated)
	ESMTeam Team;

	UPROPERTY(Replicated)
	ESMCharacterType CharacterType;
};
