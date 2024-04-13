// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SMGameplayAbility.h"
#include "SMGameplayAbility_Stun.generated.h"

/**
 * 24.04.13 수정
 * 스턴 상태를 관리하는 GA입니다. 스턴 시작시 처리와 스턴 시간이 만료되었을때 스턴 종료 요청을 수행합니다.
 * 즉, 스턴 종료 시간이 되면 현재 활성화 된 GA에 필요한 처리를 하도록 이벤트를 발생시키고 각각 GA에서 처리가 완료되면 이 스턴 GA에서 스턴 종료 관련 처리를 수행합니다.
 *
 * 스턴 타이머가 만료된 시점부터 스턴이 종료되기 전까지 잡기 불가능 태그를 통해 다시 잡히는 일이 없도록 처리합니다.
 * 
 * 스턴이 시작되면 스턴시간 만큼의 타이머를 시작합니다.
 * 스턴 시간이 만료되어 타이머에 연결된 함수가 호출되면 현재 시전자의 상태를 체크합니다. 이렇게 상태를 체크하고 다음과 같은 처리를 수행합니다.
 *      - 잡혀 있는 상태라면 CatchExit GA를 활성화하고 잡기가 풀리고 난 후 스턴 관련 처리를 수행합니다.
 *      - 매치기를 당하는 중이라면 매치기가 끝날때까지 대기 후 스턴 종료 처리를 수행합니다.
 *      - 어떤 상태에도 속하지 않으면 스턴 해제 애니메이션을 재생하고 즉시 스턴 종료 처리를 수행합니다.
 *
 * 스턴 종료에서는 다음과 같은 처리가 이루어집니다.
 *      - 트래킹 활성화
 *
 * 스턴 시작 시 처리되는 목록
 * 트래킹 비활성화
 *
 * 스턴 시작 시 추가되는 태그
 * 스턴 상태 태그
 *
 * 스턴 종료 시 제거되는 태그
 * 스턴 상태 태그
 * 잡기 불가능 태그
 */
UCLASS()
class STEREOMIX_API USMGameplayAbility_Stun : public USMGameplayAbility
{
	GENERATED_BODY()

public:
	USMGameplayAbility_Stun();

public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UFUNCTION()
	void OnStunTimeEnded();

protected:
	/** 스턴이 끝나는 시점에 스매시 중이라면 스매시의 이벤트를 기다립니다.*/
	void ProcessBuzzerBeaterSmashed();

	/** ResetSmashedState에서 이어집니다. 스매시가 종료된 경우 호출됩니다.*/
	UFUNCTION()
	void OnBuzzerBeaterSmashEnded(FGameplayEventData Payload);
	
	void ProcessCaughtExit();

	UFUNCTION()
	void OnCaughtExitEnded(FGameplayEventData Payload);
	
	/** 스턴 상태에서 벗어납니다. */
	void ResetStunState();

protected:
	UFUNCTION()
	void OnComplete();

	/** 스턴이 종료될때 필요한 처리를 합니다.. */
	void OnStunEnded();

protected:
	UPROPERTY(EditAnywhere, Category = "Montage")
	TObjectPtr<UAnimMontage> StunMontage;

	UPROPERTY(EditAnywhere, Category = "Montage")
	TObjectPtr<UAnimMontage> StandUpMontage;

	UPROPERTY(EditAnywhere, Category = "Montage")
	TObjectPtr<UAnimMontage> CatchExitMontage;

	UPROPERTY(EditAnywhere, Category = "Montage")
	TObjectPtr<UAnimMontage> StunEndMontage;

	UPROPERTY(EditAnywhere, Category = "GAS|GE")
	TArray<TSubclassOf<UGameplayEffect>> StunEndedGEs;

protected:
	UPROPERTY(EditAnywhere, Category = "Stun")
	float StunTime = 6.0f;
};
