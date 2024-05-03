// Copyright Surround, Inc. All Rights Reserved.


#include "SMGameplayCueNotify.h"

bool USMGameplayCueNotify::OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const
{
	if (bIsAttach)
	{
		PlayNiagaraSystemWithAttach(MyTarget, Parameters);
	}
	else
	{
		PlayNiagaraSystem(MyTarget, Parameters);
	}

	return true;
}
