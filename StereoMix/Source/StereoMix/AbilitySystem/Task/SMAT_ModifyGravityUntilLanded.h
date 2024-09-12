// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "SMAT_ModifyGravityUntilLanded.generated.h"

class ASMPlayerCharacterBase;

DECLARE_DELEGATE(FOnLandedDelegate);
/**
 * 착지하기 전까지 적용할 중력을 설정하는데 사용합니다. 참고로 착지시 미끄러지지 않도록 움직임을 멈춥니다.
 */
UCLASS()
class STEREOMIX_API USMAT_ModifyGravityUntilLanded : public UAbilityTask
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static USMAT_ModifyGravityUntilLanded* ModifyGravityUntilLanded(UGameplayAbility* OwningAbility, bool bUseGravityModifier = false, float GravityModifierValue = 1.0f);

	virtual void Activate() override;

	virtual void OnDestroy(bool bInOwnerFinished) override;

	FOnLandedDelegate OnLanded;

protected:
	void OnLandedCallback(ASMPlayerCharacterBase* LandedCharacter);

	TWeakObjectPtr<ASMPlayerCharacterBase> SourceCharacter;

	uint32 bChangeGravity:1 = false;
	float OriginalGravity = 0.0f;
	float TargetGravity = 0.0f;
};
