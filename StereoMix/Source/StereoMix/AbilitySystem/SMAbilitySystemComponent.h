// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"

#include "SMAbilitySystemComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnChangedTagSignature, const FGameplayTag& /*Tag*/, bool /*TagExists*/);

class ASMPlayerCharacter;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class STEREOMIX_API USMAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	/** 태그를 추가하고 리플리케이트 합니다. 서버에서만 실행되어야합니다. */
	void AddTag(const FGameplayTag& InGameplayTag);

	/** 태그를 제거하고 리플리케이트 합니다. 서버에서만 실행되어야합니다. */
	void RemoveTag(const FGameplayTag& InGameplayTag);

	/** 게임플레이 큐를 추가하고 전파합니다. */
	void AddGC(AActor* TargetActor, const FGameplayTag& GameplayCueTag, const FGameplayCueParameters& Parameters);

	/** 게임플레이 큐를 제거하고 전파합니다. */
	void RemoveGC(AActor* TargetActor, const FGameplayTag& GameplayCueTag, const FGameplayCueParameters& Parameters);
	
	/** 게임플레이 큐를 실행하고 전파합니다. */
	void ExecuteGC(AActor* TargetActor, const FGameplayTag& GameplayCueTag, const FGameplayCueParameters& Parameters);

	FOnChangedTagSignature OnChangedTag;

protected:
	virtual void OnTagUpdated(const FGameplayTag& Tag, bool TagExists) override;

	UFUNCTION(Server, Reliable)
	void ServerRequestGC(AActor* TargetActor, const FGameplayTag& GameplayCueTag, EGameplayCueEvent::Type CueEvent, const FGameplayCueParameters& Parameters, bool bExcludeSelf = true);

	UFUNCTION(Client, Reliable)
	void ClientExecuteGC(AActor* TargetActor, const FGameplayTag& GameplayCueTag, EGameplayCueEvent::Type CueEvent, const FGameplayCueParameters& Parameters);
};
