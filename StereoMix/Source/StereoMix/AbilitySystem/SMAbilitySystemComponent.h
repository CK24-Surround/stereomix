// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "SMAbilitySystemComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnChangedTagSignature, const FGameplayTag& /*Tag*/, bool /*TagExists*/);

class ASMPlayerCharacter;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STEREOMIX_API USMAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

protected:
	virtual void OnTagUpdated(const FGameplayTag& Tag, bool TagExists) override;

public:
	FOnChangedTagSignature OnChangedTag;

public:
	/** 태그를 추가하고 리플리케이트 합니다. 서버에서만 실행되어야합니다. */
	void AddTag(const FGameplayTag& InGameplayTag);

	/** 태그를 제거하고 리플리케이트 합니다. 서버에서만 실행되어야합니다. */
	void RemoveTag(const FGameplayTag& InGameplayTag);

public:
	/** GE를 기반으로 이동속도를 증가시킵니다. RemoveMoveSpeedMultiply 호출 전까지 영구히 지속 됩니다.
	 * @param MultiplyMoveSpeedGE 사용할 GE입니다.
	 * @param MoveSpeedMagnitude 이동속도 증가치입니다.
	 */
	void ApplyMoveSpeedMultiplyInfinite(const TSubclassOf<UGameplayEffect>& MultiplyMoveSpeedGE, float MoveSpeedMagnitude);

	/** ApplyMoveSpeedMultiplyInfinite를 제거하는데 사용합니다.
	 * @param RemoveMoveSpeedGE 제거할 GE입니다. 적용했던 GE를 그대로 넣어주면됩니다.
	 * @param MoveSpeedMagnitudeToRemove 예측을 위한 이동속도 감소치입니다. 적용할때 사용한 이동속도 증가치를 그대로 넣어주면됩니다.
	 */
	void RemoveMoveSpeedMultiply(const TSubclassOf<UGameplayEffect>& RemoveMoveSpeedGE, float MoveSpeedMagnitudeToRemove);

protected:
	/** MoveSpeedMultiplyInfinite의 예측 실행을 수행했는지 여부를 담고 있는 맵입니다. */
	TMap<TSubclassOf<UGameplayEffect>, bool> MoveSpeedMultiplyInfiniteLocalPredictMap;
};
