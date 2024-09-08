// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "SMAT_NextActionProccedCheck.generated.h"

class USMGA_Slash;

DECLARE_DELEGATE(FOnNextActionProccedDelegate);

/**
 * 
 */
UCLASS()
class STEREOMIX_API USMAT_NextActionProccedCheck : public UAbilityTask
{
	GENERATED_BODY()

public:
	USMAT_NextActionProccedCheck();

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static USMAT_NextActionProccedCheck* NextActionProccedCheck(UGameplayAbility* OwningAbility);

	virtual void TickTask(float DeltaTime) override;

	FOnNextActionProccedDelegate OnNextActionProcced;

protected:
	TWeakObjectPtr<USMGA_Slash> SourceAbility;
};
