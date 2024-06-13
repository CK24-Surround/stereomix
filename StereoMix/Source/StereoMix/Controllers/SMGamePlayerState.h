// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "Games/SMPlayerState.h"
#include "SMGamePlayerState.generated.h"

class USMAbilitySystemComponent;
class USMCharacterAttributeSet;
/**
 * 
 */
UCLASS()
class STEREOMIX_API ASMGamePlayerState : public ASMPlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ASMGamePlayerState();

	virtual void PostInitializeComponents() override;

public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	UPROPERTY(VisibleAnywhere, Category = "GAS|ASC")
	TObjectPtr<USMAbilitySystemComponent> ASC;

	UPROPERTY(VisibleAnywhere, Category = "GAS|AttributeSet")
	TObjectPtr<USMCharacterAttributeSet> CharacterAttributeSet;
};
