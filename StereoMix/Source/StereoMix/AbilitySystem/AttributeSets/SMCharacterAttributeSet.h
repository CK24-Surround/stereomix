// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "SMAttributeSet.h"
#include "Utilities/SMAttributeAccessor.h"
#include "SMCharacterAttributeSet.generated.h"

/**
 * 
 */
UCLASS()
class STEREOMIX_API USMCharacterAttributeSet : public USMAttributeSet
{
	GENERATED_BODY()

public:
	USMCharacterAttributeSet();

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

public:
	ATTRIBUTE_ACCESSORS(USMCharacterAttributeSet, PostureGauge);
	ATTRIBUTE_ACCESSORS(USMCharacterAttributeSet, MaxPostureGauge);
	ATTRIBUTE_ACCESSORS(USMCharacterAttributeSet, MoveSpeed);
	ATTRIBUTE_ACCESSORS(USMCharacterAttributeSet, ProjectileAttack);
	ATTRIBUTE_ACCESSORS(USMCharacterAttributeSet, ProjectileAttackCooldown);

	ATTRIBUTE_ACCESSORS(USMCharacterAttributeSet, Damage);

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

protected:
	FGameplayTagContainer InvincibleStateTags;
};
