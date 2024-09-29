// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Data/SMTeam.h"
#include "Data/SMActiveAbility.h"
#include "SMPlayerCharacterDataAsset.generated.h"

class USMUserWidget_CharacterState;
class UNiagaraSystem;
class UGameplayAbility;
class UGameplayEffect;
class ASMWeaponBase;

/**
 * 
 */
UCLASS()
class STEREOMIX_API USMPlayerCharacterDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "GAS|GE")
	TSubclassOf<UGameplayEffect> ForInitGE;

	UPROPERTY(EditAnywhere, Category = "GAS|GE")
	TSubclassOf<UGameplayEffect> SkillGaugeGE;

	UPROPERTY(EditAnywhere, Category = "GAS|GE")
	TSubclassOf<UGameplayEffect> StaminaGE;

	UPROPERTY(EditAnywhere, Category = "GAS|GE")
	TSubclassOf<UGameplayEffect> DamageGE;

	UPROPERTY(EditAnywhere, Category = "GAS|GA")
	TMap<EActiveAbility, TSubclassOf<UGameplayAbility>> DefaultActiveAbilities;

	UPROPERTY(EditAnywhere, Category = "GAS|GA")
	TArray<TSubclassOf<UGameplayAbility>> DefaultAbilities;

	UPROPERTY(EditAnywhere, Category = "GAS|GA")
	TArray<TSubclassOf<UGameplayAbility>> PassiveAbilities;

	UPROPERTY(EditAnywhere, Category = "Widget")
	TMap<ESMTeam, TSubclassOf<USMUserWidget_CharacterState>> CharacterStateWidget;

	UPROPERTY(EditAnywhere, Category = "FX")
	TMap<ESMTeam, TObjectPtr<UNiagaraSystem>> NoiseBreakIndicatorFX;

	UPROPERTY(EditAnywhere, Category = "FX")
	TMap<ESMTeam, TObjectPtr<UNiagaraSystem>> DefaultMoveTrailFX;

	UPROPERTY(EditAnywhere, Category = "FX")
	TMap<ESMTeam, TObjectPtr<UNiagaraSystem>> CatchMoveTrailFX;

	UPROPERTY(EditAnywhere, Category = "FX")
	TMap<ESMTeam, TObjectPtr<UNiagaraSystem>> ImmuneMoveTrailFX;

	UPROPERTY(EditAnywhere, Category = "Montage")
	TMap<ESMTeam, TObjectPtr<UAnimMontage>> AttackMontage;

	UPROPERTY(EditAnywhere, Category = "Montage")
	TMap<ESMTeam, TObjectPtr<UAnimMontage>> NoiseBreakMontage;

	UPROPERTY(EditAnywhere, Category = "Montage")
	TMap<ESMTeam, TObjectPtr<UAnimMontage>> NoiseBreakedMontage;

	UPROPERTY(EditAnywhere, Category = "Montage")
	TMap<ESMTeam, TObjectPtr<UAnimMontage>> HoldMontage;

	UPROPERTY(EditAnywhere, Category = "Montage")
	TMap<ESMTeam, TObjectPtr<UAnimMontage>> SkillMontage;

	UPROPERTY(EditAnywhere, Category = "Montage")
	TMap<ESMTeam, TObjectPtr<UAnimMontage>> StunMontage;

	UPROPERTY(EditAnywhere, Category = "Data")
	float MoveSpeed = 600.0f;

	UPROPERTY(EditAnywhere, Category = "Data")
	float MaxHP = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	TMap<ESMTeam, TSubclassOf<ASMWeaponBase>> WeaponClass;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	FName WeaponSocketName;
};
