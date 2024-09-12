// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "SMAT_SkillIndicator.generated.h"

class UNiagaraComponent;
class ASMPlayerCharacterBase;

/**
 * 
 */
UCLASS()
class STEREOMIX_API USMAT_SkillIndicator : public UAbilityTask
{
	GENERATED_BODY()

public:
	USMAT_SkillIndicator();

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static USMAT_SkillIndicator* SkillIndicator(UGameplayAbility* OwningAbility, ASMPlayerCharacterBase* NewSourceCharacter, UNiagaraComponent* NewIndicator, float NewMaxDistance);

protected:
	virtual void Activate() override;

	virtual void TickTask(float DeltaTime) override;

	virtual void OnDestroy(bool bInOwnerFinished) override;

	TWeakObjectPtr<ASMPlayerCharacterBase> SourceCharacter;

	TWeakObjectPtr<UNiagaraComponent> Indicator;

	float MaxDistance = 0.0f;

	uint32 bHasMaxDistance:1 = false;

	FVector CapsuleOffset;
};
