// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "StereoMixAbilitySystemComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnChangedTagSignature, const FGameplayTag& /*Tag*/, bool /*TagExists*/);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STEREOMIX_API UStereoMixAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void OnTagUpdated(const FGameplayTag& Tag, bool TagExists) override;

public:
	FOnChangedTagSignature OnChangedTag;

public:
	// 자신이 잡고 있는 폰을 반환합니다.
	FORCEINLINE APawn* GetCurrentHoldingPawn() { return CurrentHoldingPawn.Get(); }

	// 자신이 잡혀 있는 폰을 반환합니다.
	FORCEINLINE APawn* GetCurrentHolderPawn() { return CurrentHolderPawn.Get(); }

protected:
	// 자신이 잡고 있는 폰을 의미합니다.
	UPROPERTY(Replicated)
	TWeakObjectPtr<APawn> CurrentHoldingPawn;

	// 자신이 잡혀 있는 폰을 의미합니다.
	UPROPERTY(Replicated)
	TWeakObjectPtr<APawn> CurrentHolderPawn;
};
