// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayEffectTypes.h"
#include "SMUserWidget.h"

#include "SMUserWidget_CharacterState.generated.h"

class UProgressBar;
class UTextBlock;
/**
 *
 */
UCLASS(Abstract)
class STEREOMIX_API USMUserWidget_CharacterState : public USMUserWidget
{
	GENERATED_BODY()

public:
	virtual void SetASC(UAbilitySystemComponent* InASC) override;

	void SetNickname(const FString& InNickname);

	void OnChangeCurrentHealth(const FOnAttributeChangeData& OnAttributeChangeData);

	void OnChangeMaxHealth(const FOnAttributeChangeData& OnAttributeChangeData);

	void UpdateHPBar();

	float CurrentHealth = 0.0f;

	float MaxHealth = 0.0f;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TB_Nickname;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> PB_Health;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> HitAnimation;
};
