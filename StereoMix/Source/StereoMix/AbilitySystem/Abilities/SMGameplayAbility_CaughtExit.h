// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMGameplayAbility.h"
#include "SMGameplayAbility_CaughtExit.generated.h"

class ASMPlayerCharacter;

/**
 * 24.04.13 수정
 * 잡기 상태를 탈출하는 GA입니다.
 * 
 * 이 어빌리티에서는 다음과 같은 처리가 이루어집니다.
 *      - 콜리전 활성화
 *      - 무브먼트 활성화 (여기서 무브먼트는 조작에 의한 움직임이 아닌 중력 등에 의한 상호작용 활성화를 말합니다.)
 *      - 카메라 뷰 복구
 *
 * 이 어빌리티가 정상적으로 종료되면 다음과 같은 태그 변화가 발생합니다.
 * 시전 측에서 제거되는 태그
 *     잡힌 태그
 * 타겟 측에서 제거되는 태그
 *     잡기 태그
 */
UCLASS()
class STEREOMIX_API USMGameplayAbility_CaughtExit : public USMGameplayAbility
{
	GENERATED_BODY()

public:
	USMGameplayAbility_CaughtExit();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Montage")
	TObjectPtr<UAnimMontage> CaughtExitMontage;
};
