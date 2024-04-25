// Fill out your copyright notice in the Description page of Project Settings.


#include "SMGameplayCueNotify.h"

bool USMGameplayCueNotify::OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const
{
	PlayNiagaraSystem(MyTarget, Parameters);

	return true;
}
