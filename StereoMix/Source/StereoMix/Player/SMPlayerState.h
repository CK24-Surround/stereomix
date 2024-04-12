// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "SMPlayerState.generated.h"

class UStereoMixAbilitySystemComponent;
class UStereoMixCharacterAttributeSet;
/**
 * 
 */
UCLASS()
class STEREOMIX_API ASMPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ASMPlayerState();

public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	UPROPERTY(VisibleAnywhere, Category = "GAS|ASC")
	TObjectPtr<UStereoMixAbilitySystemComponent> ASC;

	UPROPERTY(VisibleAnywhere, Category = "GAS|AttributeSet")
	TObjectPtr<UStereoMixCharacterAttributeSet> CharacterAttributeSet; 
};
