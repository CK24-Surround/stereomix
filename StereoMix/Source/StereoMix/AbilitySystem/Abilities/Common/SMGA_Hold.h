// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/SMGameplayAbility.h"
#include "SMGA_Hold.generated.h"

class USMAT_SkillIndicator;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHoldSucceedDelegate);

/**
 * 
 */
UCLASS(Abstract)
class STEREOMIX_API USMGA_Hold : public USMGameplayAbility
{
	GENERATED_BODY()

public:
	USMGA_Hold();

	FOnHoldSucceedDelegate OnHoldSucceed;

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION()
	void OnHoldAnimNotifyTrigger(FGameplayEventData Payload);

	UFUNCTION(Server, Reliable)
	void ServerRPCRequestHold(const FVector_NetQuantize10& InStartLocation, const FVector_NetQuantize10& InCursorLocation);

	void OnHold(AActor* TargetActor);

	UFUNCTION(Client, Reliable)
	void ClientRPCSendHoldResult(bool bNewSuccessHold);

	void SyncPointHoldResult();

	UFUNCTION()
	void PlayResultMontage();

	/** 시전 위치입니다. */
	FVector StartLocation;

	/** 커서 위치입니다. */
	FVector CursorLocation;

	UPROPERTY(EditAnywhere, Category = "Design|Catch")
	float MaxDistance = 300.0f;

	UPROPERTY(EditAnywhere, Category = "Design|Catch")
	float LimitAngle = 135.0f;

	/** 방향에 구애받지 않고 무조건 잡을 수 있는 거리를 나타냅니다. */
	UPROPERTY(EditAnywhere, Category = "Design|Catch", DisplayName = "무조건 잡을 수 있는 거리")
	float UnconditionalHoldDistance = 150.0f;

	UPROPERTY(EditAnywhere, Category = "Design|Debug")
	uint32 bShowDebug:1 = false;

	uint32 bSuccessHold:1 = false;

	TWeakObjectPtr<USMAT_SkillIndicator> SkillIndicatorTask;
};
