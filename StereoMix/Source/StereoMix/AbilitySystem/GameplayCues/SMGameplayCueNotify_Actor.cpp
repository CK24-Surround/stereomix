// Copyright Surround, Inc. All Rights Reserved.


#include "SMGameplayCueNotify_Actor.h"

bool ASMGameplayCueNotify_Actor::OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	PlayNiagaraSystemWithAttachOnActivate(MyTarget, Parameters);

	return true;
}

bool ASMGameplayCueNotify_Actor::WhileActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	if (IsHidden())
	{
		SetActorHiddenInGame(false);
	}

	PlayNiagaraSystemWithAttachOnLoopingStart(MyTarget, Parameters);

	return true;
}

bool ASMGameplayCueNotify_Actor::OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	if (!IsHidden())
	{
		SetActorHiddenInGame(true);
	}

	PlayNiagaraSystemOnRemove(MyTarget, Parameters);

	return true;
}
