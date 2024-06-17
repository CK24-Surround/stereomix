// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMGameplayAbility.h"
#include "Data/SMTeam.h"
#include "SMGameplayAbility_Launch.generated.h"

class UNiagaraSystem;
class ASMProjectile;

/**
 * 클라이언트의 마우스 위치를 서버로 전송해 이 데이터를 기반으로 서버에서 투사체를 생성하는 어빌리티입니다.
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

	/** 클라이언트의 조준 정보를 서버로 전송합니다. */
	UFUNCTION(Server, Reliable)
	virtual void ServerRPCSendAimingData(const FVector_NetQuantize10& SourceLocation, const FVector_NetQuantizeNormal& Normal);

	virtual void ExecuteLaunchFX(int32 LaunchCount);

	/** 투사체를 발사시킵니다. 서버에서 호출됩니다. 이 함수를 오버라이드하여 발사 로직을 구성해야합니다. */
	virtual void LaunchProjectile(const FVector& InStartLocation, const FVector& InNormal) PURE_VIRTUAL(USMGameplayAbility_Launch::LaunchProjectile);

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
