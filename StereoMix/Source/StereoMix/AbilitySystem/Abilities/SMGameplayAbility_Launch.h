// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/SMTeam.h"
#include "SMGameplayAbility.h"

#include "SMGameplayAbility_Launch.generated.h"

/**
 *
 */
UCLASS(Abstract)
class STEREOMIX_API USMGameplayAbility_Launch : public USMGameplayAbility
{
	GENERATED_BODY()

public:
	USMGameplayAbility_Launch();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	/** 쿨다운을 SetByCaller로 처리하기 위한 오버라이드 입니다. */
	virtual void ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;

	UPROPERTY(EditAnywhere, Category = "Design|Montage")
	TMap<ESMTeam, TObjectPtr<UAnimMontage>> Montage;

	UPROPERTY()
	TObjectPtr<UAnimMontage> CachedMontage;

	UPROPERTY(EditAnywhere, Category = "Design|Projectile", DisplayName = "탄속")
	float ProjectileSpeed = 2500.0f;

	UPROPERTY(EditAnywhere, Category = "Design|Projectile", DisplayName = "초당 발사 속도")
	float LaunchRate = 2.5f;

	UPROPERTY(EditAnywhere, Category = "Design|Projectile", DisplayName = "최대사거리")
	float MaxDistance = 1250.0f;

	UPROPERTY(EditAnywhere, Category = "Design|Projectile", DisplayName = "대미지")
	float Damage = 20.0f;
};