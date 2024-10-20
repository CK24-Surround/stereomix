// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/SMGameplayAbility.h"
#include "SMGA_NoiseBreak.generated.h"

class ASMTile;
struct FSMCharacterNoiseBreakData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNoiseBreakSucceedDelegate, TArray<ASMTile*>, CapturedTiles);

/**
 * 
 */
UCLASS(Abstract)
class STEREOMIX_API USMGA_NoiseBreak : public USMGameplayAbility
{
	GENERATED_BODY()

public:
	USMGA_NoiseBreak();

	FOnNoiseBreakSucceedDelegate OnNoiseBreakSucceed;

protected:
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	bool IsValidTarget() const;

	/**
	 * 버스트 공격을 수행합니다.
	 * @param GCTag 히트 이펙트에 사용할 게임플레이 큐 태그
	 */
	virtual void PerformBurstAttack(const FVector& TargetLocation, const FGameplayTag& GCTag);

	virtual TArray<AActor*> GetBurstHitActors(const FVector& TargetLocation);

	/** 해당 위치의 타일을 가져옵니다. */
	ASMTile* GetTileFromLocation(const FVector& Location) const;

	/** 노이즈 브레이크의 데미지량 입니다.*/
	float Damage = 0.0f;

	/** 중심 기준 점령되는 타일 사이즈입니다. */
	int32 CaptureSize = 0;

	/** 타일기준 최대 사거리를 나타냅니다. */
	float MaxDistanceByTile = 0.0f;
};
