// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "StereoMixPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class STEREOMIX_API AStereoMixPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AStereoMixPlayerState();

public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	UPROPERTY(VisibleAnywhere, Category = "GAS|ASC")
	TObjectPtr<UAbilitySystemComponent> ASC;
};
