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

protected:
	virtual void OnTagUpdated(const FGameplayTag& Tag, bool TagExists) override;

public:
	FOnChangedTagSignature OnChangedTag;
};
