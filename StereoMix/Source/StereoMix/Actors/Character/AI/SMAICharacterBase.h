// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Character/SMCharacterBase.h"
#include "Interfaces/SMDamageInterface.h"
#include "Interfaces/SMHoldInteractionInterface.h"
#include "Interfaces/SMTeamInterface.h"
#include "SMAICharacterBase.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;
class USMUserWidget_CharacterState;
class UWidgetComponent;
class USMHIC_TutorialAI;
class ASMNoteBase;
class USMPlayerCharacterDataAsset;
class ASMWeaponBase;

UCLASS()
class STEREOMIX_API ASMAICharacterBase : public ASMCharacterBase, public ISMTeamInterface, public ISMHoldInteractionInterface, public ISMDamageInterface
{
	GENERATED_BODY()

public:
	ASMAICharacterBase();

	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* NewController) override;

	virtual USMHoldInteractionComponent* GetHoldInteractionComponent() const override;

	virtual USMTeamComponent* GetTeamComponent() const override { return TeamComponent; }

	virtual ESMTeam GetTeam() const override;

	virtual AActor* GetLastAttacker() const override { return LastAttacker.Get(); }

	virtual void SetLastAttacker(AActor* NewAttacker) override { LastAttacker = NewAttacker; }

	virtual bool CanIgnoreAttack() const override;

	virtual bool IsObstacle() override { return false; }

	UFUNCTION(BlueprintCallable, Category = "Note")
	bool IsNoteState() const { return bIsNoteState; }

	UFUNCTION(BlueprintCallable, Category = "Attack")
	bool CanAttack() const { return bCanAttack; }

	UFUNCTION(BlueprintCallable, Category = "Attack")
	virtual void Attack(AActor* AttackTarget) {}

	virtual void SetGhostMaterial(float Duration);

	virtual void AddScreenIndicatorToSelf(AActor* TargetActor);

	virtual void RemoveScreenIndicatorFromSelf(AActor* TargetActor);

	virtual void DestroyNoteStateEndTimer();

	virtual void RegisterNoteStateEndTimer(float Duration);

	virtual void SetNoteState(bool bNewIsNote);

	ASMNoteBase* GetNote() const { return Note; }

	ASMWeaponBase* GetWeapon() const { return Weapon; }

	TArray<UMaterialInterface*> GetOriginalMaterials() { return OriginalMaterials; }

	UMaterialInterface* GetOriginalOverlayMaterial() { return OriginalOverlayMaterial; }

protected:
	virtual void ReceiveDamage(AActor* NewAttacker, float InDamageAmount) override;

	virtual void OnChangeHP();

	/** 캐릭터의 원본 머티리얼입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Design|Material")
	TArray<TObjectPtr<UMaterialInterface>> OriginalMaterials;

	/** 캐릭터의 원본 오버레이 머티리얼입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Design|Material")
	TObjectPtr<UMaterialInterface> OriginalOverlayMaterial;

	UPROPERTY(VisibleAnywhere, Category = "HitBox")
	TObjectPtr<UCapsuleComponent> HitBox;

	UPROPERTY(VisibleAnywhere, Category = "Team")
	TObjectPtr<USMTeamComponent> TeamComponent;

	UPROPERTY(VisibleAnywhere, Category = "Widget")
	TObjectPtr<UWidgetComponent> CharacterStateWidgetComponent;

	UPROPERTY(VisibleAnywhere, Category = "Note")
	TObjectPtr<USceneComponent> NoteSlotComponent;

	UPROPERTY(EditAnywhere, Category = "Design|Widget")
	TSubclassOf<USMUserWidget_CharacterState> CharacterStateWidgetClass;

	UPROPERTY(EditAnywhere, Category = "Design|Weapon")
	TSubclassOf<ASMWeaponBase> WeaponClass;

	UPROPERTY(EditAnywhere, Category = "Design|Weapon")
	FName WeaponSocketName;

	UPROPERTY(EditAnywhere, Category = "Design|Note")
	TSubclassOf<ASMNoteBase> NoteClass;

	UPROPERTY(EditAnywhere, Category = "Design")
	float HP = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Design|VFX")
	TObjectPtr<UNiagaraSystem> ImmuneStartVFX;

	UPROPERTY(EditAnywhere, Category = "Design|VFX")
	TObjectPtr<UNiagaraSystem> ImmuneEndVFX;

	UPROPERTY(EditAnywhere, Category = "Design|VFX")
	TObjectPtr<UMaterialInterface> ImmuneMaterial;

	UPROPERTY(EditAnywhere, Category = "Design|VFX")
	TObjectPtr<UMaterialInterface> ImmuneOverlayMaterial;

	UPROPERTY()
	TObjectPtr<ASMWeaponBase> Weapon;

	UPROPERTY()
	TObjectPtr<ASMNoteBase> Note;

	UPROPERTY()
	TObjectPtr<USMHIC_TutorialAI> HIC;

	UPROPERTY()
	TObjectPtr<UNiagaraComponent> VFXComponent;

	FTimerHandle NoteStateEndTimerHandle;

	TWeakObjectPtr<AActor> LastAttacker;

	float CurrentHP;

	uint32 bIsNoteState:1 = false;

	uint32 bCanAttack:1 = true;
};
