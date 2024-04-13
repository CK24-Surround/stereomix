// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SMGameplayAbility.h"
#include "SMGameplayAbility_Stun.generated.h"

/**
 * 
 */
UCLASS()
class STEREOMIX_API USMGameplayAbility_Stun : public USMGameplayAbility
{
	GENERATED_BODY()

public:
	USMGameplayAbility_Stun();

public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UFUNCTION()
	void OnStunTimeEnded();

protected:
	/** 스턴이 끝나는 시점에 스매시 중이라면 스매시의 이벤트를 기다립니다.*/
	void ProcessBuzzerBeaterSmashed();

	/** ResetSmashedState에서 이어집니다. 스매시가 종료된 경우 호출됩니다.*/
	UFUNCTION()
	void OnBuzzerBeaterSmashEnded(FGameplayEventData Payload);

	/** 잡힌 상태에서 벗어납니다. */
	void ResetCaughtState();

	/** 타겟 캐릭터로부터 소스 캐릭터를 디태치합니다. */
	void DetachFromTargetCharacter(ASMPlayerCharacter* InTargetCharacter);
	
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
