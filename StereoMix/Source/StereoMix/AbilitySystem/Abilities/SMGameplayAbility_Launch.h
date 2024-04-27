// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SMGameplayAbility.h"
#include "Utilities/SMTeam.h"
#include "SMGameplayAbility_Launch.generated.h"

class UNiagaraSystem;
class ASMProjectile;

/**
 * 클라이언트의 마우스 위치를 서버로 전송해 이 데이터를 기반으로 서버에서 투사체를 생성하는 어빌리티입니다.
 */
UCLASS()
class STEREOMIX_API USMGameplayAbility_Launch : public USMGameplayAbility
{
	GENERATED_BODY()

public:
	USMGameplayAbility_Launch();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	/** 쿨다운을 SetByCaller로 처리하기 위한 오버라이드 입니다. */
	virtual void ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;

protected:
	/** 클라이언트의 조준 값을 받아오는데 사용합니다. */
	virtual void OnReceiveProjectileTargetData(const FGameplayAbilityTargetDataHandle& GameplayAbilityTargetDataHandle, FGameplayTag GameplayTag);

protected:
	UPROPERTY(EditAnywhere, Category = "Montage")
	TObjectPtr<UAnimMontage> Montage;

	UPROPERTY(EditAnywhere, Category = "Projectile", DisplayName = "탄속")
	float ProjectileSpeed = 2500.0f;
	
	UPROPERTY(EditAnywhere, Category = "Projectile", DisplayName = "초당 발사 속도")
	float LaunchRate = 2.5f;

	UPROPERTY(EditAnywhere, Category = "Projectile", DisplayName = "최대사거리")
	float MaxDistance = 1250.0f;

	UPROPERTY(EditAnywhere, Category = "Projectile", DisplayName = "대미지")
	float Damage = 20.0f;
};
