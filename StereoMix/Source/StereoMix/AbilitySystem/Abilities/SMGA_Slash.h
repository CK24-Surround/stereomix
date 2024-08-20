// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMGameplayAbility.h"
#include "SMGA_Slash.generated.h"

struct FSlashData
{
	TArray<float> SlashYawOffsets;

	int32 Count = 0;
	const int32 SegmentCount = 30;
	float Rate = 0.0f;
	uint32 bIsReverse:1 = false;
};

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

	virtual void ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;

	void Smash();

	void CalculateCurrentYaw();

	void SendCurrentYaw();

	UFUNCTION(Server, Reliable)
	void ServerRPCSendCurrentYaw(float Yaw);

	TArray<AActor*> GetDetectedTarget(float Yaw);

	UPROPERTY(EditAnywhere, Category = "Design")
	float Cooldown = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Design")
	float Distance = 750.0f;

	UPROPERTY(EditAnywhere, Category = "Design")
	float Angle = 180.0f;

	UPROPERTY(EditAnywhere, Category = "Design")
	float SlashTime = 0.25f;

	FSlashData SlashData;
};
