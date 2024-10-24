// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "SMAT_OnTick.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FSMOnTick, float DeltaTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSMOnTickDynamic, float, DeltaTime);


UCLASS()
class STEREOMIX_API USMAT_OnTick : public UAbilityTask
{
	GENERATED_BODY()

public:
	USMAT_OnTick();

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static USMAT_OnTick* AbilityTaskOnTick(UGameplayAbility* OwningAbility, FName TaskInstanceName);

	virtual void Activate() override;
	virtual void TickTask(float DeltaTime) override;

	FSMOnTick OnTick;

private:
	UPROPERTY(BlueprintAssignable, Category = "Ability|Tasks", meta = (DisplayName = "OnTick"))
	FSMOnTickDynamic K2_OnTick;
};
