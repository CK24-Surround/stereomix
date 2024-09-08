// Copyright Surround, Inc. All Rights Reserved.


#include "SMAT_NextActionProccedCheck.h"

#include "AbilitySystem/Abilities/SMGA_Slash.h"

USMAT_NextActionProccedCheck::USMAT_NextActionProccedCheck()
{
	bTickingTask = true;
}

USMAT_NextActionProccedCheck* USMAT_NextActionProccedCheck::NextActionProccedCheck(UGameplayAbility* OwningAbility)
{
	USMAT_NextActionProccedCheck* NewObj = NewAbilityTask<USMAT_NextActionProccedCheck>(OwningAbility);
	NewObj->SourceAbility = Cast<USMGA_Slash>(OwningAbility);
	return NewObj;
}

void USMAT_NextActionProccedCheck::TickTask(float DeltaTime)
{
	if (!SourceAbility.Get())
	{
		return;
	}

	if (SourceAbility->bCanProceedNextAction && SourceAbility->bIsInput)
	{
		SourceAbility->bCanProceedNextAction = false;
		SourceAbility->bIsInput = false;

		if (ShouldBroadcastAbilityTaskDelegates())
		{
			(void)OnNextActionProcced.ExecuteIfBound();
		}
	}
}

void USMAT_NextActionProccedCheck::OnDestroy(bool bInOwnerFinished)
{
	OnNextActionProcced.Unbind();

	Super::OnDestroy(bInOwnerFinished);
}
