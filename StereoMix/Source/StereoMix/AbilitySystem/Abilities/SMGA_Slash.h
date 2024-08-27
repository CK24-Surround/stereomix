// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMGameplayAbility.h"
#include "SMGA_Slash.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class STEREOMIX_API USMGA_Slash : public USMGameplayAbility
{
	GENERATED_BODY()

public:
	USMGA_Slash();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

	void Slash();

	void SlashEnded();

	void SlashRequest();

	UFUNCTION(Server, Reliable)
	void ServerRPCSlashRequest();

	UFUNCTION()
	void SlashAnimationEnded();

	UPROPERTY(EditAnywhere, Category = "Design")
	float Distance = 500.0f;

	UPROPERTY(EditAnywhere, Category = "Design")
	float Angle = 240.0f;

	UPROPERTY(EditAnywhere, Category = "Design")
	float SlashTime = 0.1875f;

// 임시
protected:
	UFUNCTION()
	void CancelledTest();

	UFUNCTION()
	void InterruptedTest();

	UFUNCTION()
	void CompletedTest();
};
