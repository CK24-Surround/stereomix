// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMGameplayAbility_Launch.h"

#include "SMGameplayAbility_Launch_Single.generated.h"

/**
 *
 */
UCLASS(Abstract)
class STEREOMIX_API USMGameplayAbility_Launch_Single : public USMGameplayAbility_Launch
{
	GENERATED_BODY()

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	/** 클라이언트의 조준 정보를 서버로 전송합니다. */
	UFUNCTION(Server, Reliable)
	virtual void ServerRPCSendAimingData(const FVector_NetQuantize10& SourceLocation, const FVector_NetQuantizeNormal& Normal);

	virtual void ExecuteLaunchFX(int32 LaunchCount);

	/** 투사체를 발사시킵니다. 서버에서 호출됩니다. 이 함수를 오버라이드하여 발사 로직을 구성해야합니다. */
	virtual void LaunchProjectile(const FVector& InStartLocation, const FVector& InNormal) PURE_VIRTUAL(USMGameplayAbility_Launch::LaunchProjectile);
};