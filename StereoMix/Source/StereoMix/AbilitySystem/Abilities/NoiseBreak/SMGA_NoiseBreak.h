// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/SMGameplayAbility.h"
#include "Tiles/SMTile.h"
#include "SMGA_NoiseBreak.generated.h"

struct FSMCharacterNoiseBreakData;
/**
 * 
 */
UCLASS(Abstract)
class STEREOMIX_API USMGA_NoiseBreak : public USMGameplayAbility
{
	GENERATED_BODY()

public:
	USMGA_NoiseBreak();

protected:
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	bool IsValidTarget() const;

	/**
	 * 스플래시 데미지를 적용할때 호출합니다.
	 * @param GCTag 히트 이펙트에 사용할 게임플레이 큐 태그
	 */
	virtual void ApplySplash(const FVector& TargetLocation, const FGameplayTag& GCTag);
	
	/** 해당 위치의 타일을 가져옵니다. */
	ASMTile* GetTileFromLocation(const FVector& Location);

	/** 노이즈 브레이크의 데미지량 입니다.*/
	float Damage = 0.0f;

	/** 중심 기준 점령되는 타일 사이즈입니다. */
	int32 CaptureSize = 0;

	/** 타일기준 최대 사거리를 나타냅니다. */
	float MaxDistanceByTile = 0.0f;

private:
	TArray<AActor*> GetSplashHitActors(const FVector& TargetLocation);

	bool CanApplySplashDamage(AActor* TargetActor);
};
