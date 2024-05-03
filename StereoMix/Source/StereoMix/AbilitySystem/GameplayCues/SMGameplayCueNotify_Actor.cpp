// Copyright Surround, Inc. All Rights Reserved.


#include "SMGameplayCueNotify_Actor.h"

#include "NiagaraComponent.h"

bool ASMGameplayCueNotify_Actor::OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	PlayOnActivateNiagaraSystemWithAttach(MyTarget, Parameters);

	return true;
}

bool ASMGameplayCueNotify_Actor::WhileActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	if (IsHidden())
	{
		SetActorHiddenInGame(false);
	}

	PlayOnLoopingStartNiagaraSystemWithAttach(MyTarget, Parameters);

	return true;
}

bool ASMGameplayCueNotify_Actor::OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	if (LoopingFXComponent)
	{
		LoopingFXComponent->DeactivateImmediate();
	}

	if (!IsHidden())
	{
		SetActorHiddenInGame(true);
	}

	PlayOnRemoveNiagaraSystemWithAttach(MyTarget, Parameters);

	return true;
}
