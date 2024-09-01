// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMGameplayAbility.h"
#include "Data/SMTeam.h"
#include "SMGA_Hold.generated.h"

/**
 * 
 */
UCLASS()
class STEREOMIX_API USMGA_Hold : public USMGameplayAbility
{
	GENERATED_BODY()

public:
	USMGA_Hold();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION()
	void OnHoldAnimNotifyTrigger(FGameplayEventData Payload);

	UFUNCTION(Server, Reliable)
	void ServerRPCRequestCatch(const FVector_NetQuantize10& InStartLocation, const FVector_NetQuantize10& InCursorLocation);

	/** 잡을 수 있는 가장 적합한 타겟을 반환합니다.*/
	AActor* GetMostSuitableCatchableActor(const TArray<FOverlapResult>& InOverlapResults);

	/** 오버랩 결과에서 잡을 수 있는 액터를 추려냅니다. 잡을 수 있는 캐릭터가 없다면 false를 반환합니다. */
	bool GetCatchableActors(const TArray<FOverlapResult>& InOverlapResults, TArray<AActor*>& OutCatchableActors);

	/** 타겟이 유효한 각도 내에 존재하는 지 체크합니다. */
	bool IsValidateAngle(const AActor* TargetActor);

	/** 지정된 위치에 가장 가까운 캐릭터를 얻어냅니다. */
	AActor* GetClosestActorFromLocation(const TArray<AActor*>& InActors, const FVector& InLocation);

	/** 시전 위치입니다. */
	FVector StartLocation;

	/** 커서 위치입니다. */
	FVector TargetLocation;

	UPROPERTY()
	TObjectPtr<UAnimMontage> CachedCatchMontage;

	UPROPERTY(EditAnywhere, Category = "Design|Catch")
	float MaxDistance = 300.0f;

	UPROPERTY(EditAnywhere, Category = "Design|Catch")
	float LimitDegree = 90.0f;

	/** 방향에 구애받지 않고 무조건 잡을 수 있는 거리를 나타냅니다. */
	UPROPERTY(EditAnywhere, Category = "Design|Catch", DisplayName = "무조건 잡을 수 있는 거리")
	float UnconditionallyCatchDistance = 150.0f;

	UPROPERTY(EditAnywhere, Category = "Design|Debug")
	uint32 bShowDebug:1 = false;
};
