// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMGA_Attack.h"
#include "SMGA_Archery.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class STEREOMIX_API USMGA_Archery : public USMGA_Attack
{
	GENERATED_BODY()

public:
	USMGA_Archery();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

	UFUNCTION(Server, Reliable)
	void ServerLaunchProjectile(const FVector_NetQuantize10& SourceLocation, const FVector_NetQuantize10& TargetLocation, int32 InChargedLevel);

	float ProjectileSpeed = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Design")
	float Charge1DamageMultiply = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Design")
	float Charge2DamageMultiply = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Design")
	float TimeToChargeLevel1 = 0.4f;

	UPROPERTY(EditAnywhere, Category = "Design")
	float TimeToChargeLevel2 = 0.9f;

	int32 ChargedLevel = 0;

	FTimerHandle Level1ChargedTimerHandle;

	FTimerHandle Level2ChargedTimerHandle;
};
