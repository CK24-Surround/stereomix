// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "SMAT_CheckTeamTileEntryExit.generated.h"

class ASMPlayerCharacterBase;

DECLARE_DELEGATE(FOnTeamTileInOutDelegate);

/**
 * 
 */
UCLASS()
class STEREOMIX_API USMAT_CheckTeamTileEntryExit : public UAbilityTask
{
	GENERATED_BODY()

public:
	USMAT_CheckTeamTileEntryExit();

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static USMAT_CheckTeamTileEntryExit* CheckTeamTileEntryExit(UGameplayAbility* OwningAbility, ASMPlayerCharacterBase* NewSourceCharacter);

	FOnTeamTileInOutDelegate OnTeamTileEntry;

	FOnTeamTileInOutDelegate OnTeamTileExit;

protected:
	virtual void TickTask(float DeltaTime) override;

	void SetIsEntry(bool bNewIsEntry);

	UFUNCTION()
	void OnTeamTileEntryCallback();

	UFUNCTION()
	void OnTeamTileExitCallback();

	TWeakObjectPtr<ASMPlayerCharacterBase> SourceCharacter;

	uint32 bIsEntry:1 = false;
};
