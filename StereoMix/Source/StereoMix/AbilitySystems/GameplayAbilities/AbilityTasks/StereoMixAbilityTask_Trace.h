// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "StereoMixAbilityTask_Trace.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCompleteTraceSignature, const FGameplayAbilityTargetDataHandle&, TargetDataHandle);

class AGameplayAbilityTargetActor;
/**
 * 
 */
UCLASS()
class STEREOMIX_API UStereoMixAbilityTask_Trace : public UAbilityTask
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", DisplayName = "Tarce", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"))
	static UStereoMixAbilityTask_Trace* CreateTask(UGameplayAbility* OwningAbility, const FVector& StartLocation, const FVector& StartDirection);

protected:
	virtual void Activate() override;
	
	virtual void OnDestroy(bool bInOwnerFinished) override;

protected:
	void BeginSpawningActor();
	
	void FinishSpawningActor();

public:
	FOnCompleteTraceSignature OnCompleteTrace;
	
protected:
	UPROPERTY(EditAnywhere, Category = "TargetActor")
	TSubclassOf<AGameplayAbilityTargetActor> TargetActorClass;
};
