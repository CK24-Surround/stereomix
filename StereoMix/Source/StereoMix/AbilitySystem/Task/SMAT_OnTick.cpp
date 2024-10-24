// Copyright Studio Surround. All Rights Reserved.


#include "SMAT_OnTick.h"

USMAT_OnTick::USMAT_OnTick()
{
	bTickingTask = true;
}

USMAT_OnTick* USMAT_OnTick::AbilityTaskOnTick(UGameplayAbility* OwningAbility, FName TaskInstanceName)
{
	USMAT_OnTick* MyObj = NewAbilityTask<USMAT_OnTick>(OwningAbility, TaskInstanceName);
	return MyObj;
}

void USMAT_OnTick::Activate()
{
	Super::Activate();
}

void USMAT_OnTick::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnTick.Broadcast(DeltaTime);
		K2_OnTick.Broadcast(DeltaTime);
	}
}
