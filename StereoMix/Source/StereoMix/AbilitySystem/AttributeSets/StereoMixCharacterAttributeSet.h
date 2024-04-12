// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "StereoMixAttributeSet.h"
#include "Utilities/SMAttributeAccessor.h"
#include "StereoMixCharacterAttributeSet.generated.h"

/**
 * 
 */
UCLASS()
class STEREOMIX_API UStereoMixCharacterAttributeSet : public UStereoMixAttributeSet
{
	GENERATED_BODY()

public:
	UStereoMixCharacterAttributeSet();

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

protected:
	FGameplayTag StunEventTag;

public:
	ATTRIBUTE_ACCESSORS(UStereoMixCharacterAttributeSet, PostureGauge);
	ATTRIBUTE_ACCESSORS(UStereoMixCharacterAttributeSet, MaxPostureGauge);
	ATTRIBUTE_ACCESSORS(UStereoMixCharacterAttributeSet, MoveSpeed);
	ATTRIBUTE_ACCESSORS(UStereoMixCharacterAttributeSet, ProjectileAttack);
	ATTRIBUTE_ACCESSORS(UStereoMixCharacterAttributeSet, ProjectileAttackCooldown);

	ATTRIBUTE_ACCESSORS(UStereoMixCharacterAttributeSet, Damage);

protected:
	UFUNCTION()
	void OnRep_PostureGauge(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxPostureGauge(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MoveSpeed(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_ProjectileAttack(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_ProjectileAttackCooldown(const FGameplayAttributeData& OldValue);

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Posture", ReplicatedUsing = "OnRep_PostureGauge")
	FGameplayAttributeData PostureGauge;

	UPROPERTY(BlueprintReadOnly, Category = "Posture", ReplicatedUsing = "OnRep_MaxPostureGauge")
	FGameplayAttributeData MaxPostureGauge;

	UPROPERTY(BlueprintReadOnly, Category = "Posture", ReplicatedUsing = "OnRep_MoveSpeed")
	FGameplayAttributeData MoveSpeed;

	UPROPERTY(BlueprintReadOnly, Category = "Posture", ReplicatedUsing = "OnRep_ProjectileAttack")
	FGameplayAttributeData ProjectileAttack;

	UPROPERTY(BlueprintReadOnly, Category = "Posture", ReplicatedUsing = "OnRep_ProjectileAttackCooldown")
	FGameplayAttributeData ProjectileAttackCooldown;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	FGameplayAttributeData Damage;
};
