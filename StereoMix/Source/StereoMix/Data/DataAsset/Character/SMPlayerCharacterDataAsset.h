// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Data/SMTeam.h"
#include "Data/SMActiveAbility.h"
#include "SMPlayerCharacterDataAsset.generated.h"

class ASMNoteBase;
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

	UPROPERTY(EditAnywhere, Category = "GAS|GE")
	TSubclassOf<UGameplayEffect> StaminaHealGE;

	UPROPERTY(EditAnywhere, Category = "GAS|GE")
	TSubclassOf<UGameplayEffect> ResetStaminaGE;

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
	TMap<ESMTeam, TObjectPtr<UNiagaraSystem>> SkillIndicatorVFX;

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

	UPROPERTY(EditAnywhere, Category = "Weapon")
	TMap<ESMTeam, TSubclassOf<ASMWeaponBase>> WeaponClass;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	FName WeaponSocketName;

	UPROPERTY(EditAnywhere, Category = "Note")
	TMap<ESMTeam, TSubclassOf<ASMNoteBase>> NoteClass;

	UPROPERTY(EditAnywhere, Category = "CameraShake")
	TSubclassOf<UCameraShakeBase> HitCameraShake;

	UPROPERTY(EditAnywhere, Category = "CameraShake")
	TSubclassOf<UCameraShakeBase> SkillHitCameraShake;

	UPROPERTY(EditAnywhere, Category = "CameraShake")
	TSubclassOf<UCameraShakeBase> NoiseBreakCameraShake;

	UPROPERTY(EditAnywhere, Category = "Curve")
	TObjectPtr<UCurveFloat> BlinkCurve;

	UPROPERTY(EditAnywhere, Category = "Curve")
	TObjectPtr<UCurveFloat> MouthCurve;
};
