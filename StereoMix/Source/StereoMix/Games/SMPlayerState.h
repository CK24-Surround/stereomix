// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Data/SMCharacterType.h"
#include "Data/SMTeam.h"

#include "SMPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTeamChangedEvent, ESMTeam, NewTeam);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCharacterTypeChangedEvent, ESMCharacterType, NewCharacterType);

/**
 *
 */
UCLASS(Abstract)
class STEREOMIX_API ASMPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void SeamlessTravelTo(APlayerState* NewPlayerState) override;

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// ============================================================================
	// Team, Character Type

private:
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_Team)
	ESMTeam Team;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_CharacterType)
	ESMCharacterType CharacterType;

	UFUNCTION()
	void OnRep_Team(ESMTeam PreviousTeam);

	UFUNCTION()
	void OnRep_CharacterType(ESMCharacterType PreviousCharacterType);

public:
	UPROPERTY(BlueprintAssignable)
	FTeamChangedEvent TeamChangedEvent;

	UPROPERTY(BlueprintAssignable)
	FCharacterTypeChangedEvent CharacterTypeChangedEvent;

	UFUNCTION(BlueprintCallable)
	FORCEINLINE ESMTeam GetTeam() const { return Team; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE ESMCharacterType GetCharacterType() const { return CharacterType; }

	bool SetTeam(ESMTeam NewTeam);

	bool SetCharacterType(ESMCharacterType NewCharacterType);

protected:
	virtual bool CanChangeTeam(ESMTeam NewTeam) const;
	virtual bool CanChangeCharacterType(ESMCharacterType NewCharacterType) const;

	virtual void OnTeamChanged(ESMTeam PreviousTeam, ESMTeam NewTeam);
	virtual void OnCharacterTypeChanged(ESMCharacterType PreviousCharacterType, ESMCharacterType NewCharacterType);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = PlayerState)
	bool bCopyTeamOnSeamlessTravel = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = PlayerState)
	bool bCopyCharacterTypeOnSeamlessTravel = true;
};
