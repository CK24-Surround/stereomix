// Fill out your copyright notice in the Description page of Project Settings.


#include "StereoMixAbilityTask_Trace.h"

#include "Utilities/StereoMixeLog.h"

UStereoMixAbilityTask_Trace* UStereoMixAbilityTask_Trace::CreateTask(UGameplayAbility* OwningAbility, const FVector& StartLocation, const FVector& StartDirection)
{
	if (!OwningAbility)
	{
		NET_LOG(nullptr, Error, TEXT("Task 생성에 실패했습니다: OwningAbility가 유효하지 않습니다."));
		return nullptr;
	}

	UStereoMixAbilityTask_Trace* NewTask = NewAbilityTask<UStereoMixAbilityTask_Trace>(OwningAbility);
	if (!NewTask)
	{
		NET_LOG(nullptr, Error, TEXT("Task 생성에 실패했습니다."));
		return nullptr;
	}

	return NewTask;
}

void UStereoMixAbilityTask_Trace::Activate()
{
	Super::Activate();
}

void UStereoMixAbilityTask_Trace::OnDestroy(bool bInOwnerFinished)
{
	Super::OnDestroy(bInOwnerFinished);
}

void UStereoMixAbilityTask_Trace::BeginSpawningActor()
{
	
}

void UStereoMixAbilityTask_Trace::FinishSpawningActor()
{
	
}
