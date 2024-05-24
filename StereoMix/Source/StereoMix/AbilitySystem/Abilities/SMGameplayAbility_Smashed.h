// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMGameplayAbility.h"
#include "Data/SMTeam.h"
#include "SMGameplayAbility_Smashed.generated.h"

/**
 * 24.04.13 수정
 * 스매시 피격 애니메이션과 넉다운 애니메이션을 재생하기 위한 GA입니다.
 * 
 * 버저 비티 엔드 이벤트를 발생 시킵니다.
 *     -> 해당 스매시가 버저 비티인 경우 해당 공격이 끝날때까지 지연시키기 위해 사용됩니다.
 */
UCLASS()
class STEREOMIX_API USMGameplayAbility_Smashed : public USMGameplayAbility
{
	GENERATED_BODY()

public:
	USMGameplayAbility_Smashed();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Montage")
	TMap<ESMTeam, TObjectPtr<UAnimMontage>> SmashedMontage;

	UPROPERTY()
	TObjectPtr<UAnimMontage> CachedSmashedMontage;
};
