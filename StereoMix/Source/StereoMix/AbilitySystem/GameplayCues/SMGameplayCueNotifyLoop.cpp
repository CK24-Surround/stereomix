// Copyright Surround, Inc. All Rights Reserved.


#include "SMGameplayCueNotifyLoop.h"

#include "NiagaraComponent.h"

bool ASMGameplayCueNotifyLoop::OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	OnStartFX(MyTarget, Parameters);

	return true;
}

bool ASMGameplayCueNotifyLoop::WhileActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	if (IsHidden())
	{
		SetActorHiddenInGame(false);
	}

	OnLoopingStartFX(MyTarget, Parameters);

	return true;
}

bool ASMGameplayCueNotifyLoop::OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	// 루프 애니메이션을 종료합니다.
	if (LoopingFXComponent)
	{
		LoopingFXComponent->DeactivateImmediate();
	}

	if (!IsHidden())
	{
		SetActorHiddenInGame(true);
	}

	OnEndFX(MyTarget, Parameters);

	return true;
}
