// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "SMAT_CheckUnderTile.generated.h"

class ASMPlayerCharacterBase;
class ASMTile;

DECLARE_DELEGATE_OneParam(FOnUnderTileChangedDelegate, ASMTile* /*UnderTile*/);
/**
 * 
 */
UCLASS()
class STEREOMIX_API USMAT_CheckUnderTile : public UAbilityTask
{
	GENERATED_BODY()

public:
	USMAT_CheckUnderTile();

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static USMAT_CheckUnderTile* CheckUnderTile(UGameplayAbility* OwningAbility, ASMPlayerCharacterBase* NewSourceCharacter, float TickRate);

	FOnUnderTileChangedDelegate OnUnderTileChanged;

protected:
	virtual void TickTask(float DeltaTime) override;

	virtual void OnDestroy(bool bInOwnerFinished) override;

	TWeakObjectPtr<ASMPlayerCharacterBase> SourceCharacter;

	TWeakObjectPtr<ASMTile> LastCheckedTile;

	float AccumulatedTime = 0.0f;

	float TickInterval = 0.0f;
};
