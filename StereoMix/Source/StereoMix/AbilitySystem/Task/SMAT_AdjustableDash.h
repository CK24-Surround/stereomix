// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "SMAT_AdjustableDash.generated.h"

class ASMPlayerCharacterBase;
/**
 * 
 */
UCLASS()
class STEREOMIX_API USMAT_AdjustableDash : public UAbilityTask
{
	GENERATED_BODY()

public:
	USMAT_AdjustableDash();

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static USMAT_AdjustableDash* AdjustableDash(UGameplayAbility* OwningAbility, float RotationPerSecond);

protected:
	virtual void TickTask(float DeltaTime) override;

	TWeakObjectPtr<ASMPlayerCharacterBase> SourceCharacter;

	float RotationPerSecond = 0.0f;
};
