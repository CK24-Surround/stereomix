// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMGameplayAbility.h"
#include "SMGameplayAbility_Caught.generated.h"

/**
 * 잡힌 상태 애니메이션을 재생하는 어빌리티입니다.
 * 잡혀 있는 동안 잡힌 애니메이션을 재생하기 위해서 사용됩니다.
 */
UCLASS()
class STEREOMIX_API USMGameplayAbility_Caught : public USMGameplayAbility
{
	GENERATED_BODY()

public:
	USMGameplayAbility_Caught();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Montage")
	TObjectPtr<UAnimMontage> CaughtMontage;
};
