// Copyright Surround, Inc. All Rights Reserved.


#include "SMGameplayCueNotify.h"

bool USMGameplayCueNotify::OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const
{
	PlayNiagaraSystem(MyTarget, Parameters);

	return true;
}
