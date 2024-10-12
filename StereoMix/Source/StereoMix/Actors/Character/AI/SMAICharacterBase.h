// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Character/SMCharacterBase.h"
#include "Interfaces/SMDamageInterface.h"
#include "Interfaces/SMHoldInteractionInterface.h"
#include "Interfaces/SMTeamInterface.h"
#include "SMAICharacterBase.generated.h"

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

	virtual void ReceiveDamage(AActor* NewAttacker, float InDamageAmount) override;

	virtual bool CanIgnoreAttack() const override { return false; }

	virtual bool IsObstacle() override { return false; }

	virtual void AddScreenIndicatorToSelf(AActor* TargetActor);

	virtual void RemoveScreenIndicatorFromSelf(AActor* TargetActor);

	virtual void OnChangeHP();

	UFUNCTION(BlueprintCallable, Category = "Note")
	virtual bool IsNoteState() const { return bIsNoteState; }

	UFUNCTION(BlueprintCallable, Category = "Attack")
	virtual void Attack(AActor* AttackTarget) {}
	
	virtual void SetNoteState(bool bNewIsNote);

	virtual ASMNoteBase* GetNote() const { return Note; }

protected:
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

	UPROPERTY()
	TObjectPtr<ASMWeaponBase> Weapon;

	UPROPERTY()
	TObjectPtr<ASMNoteBase> Note;

	UPROPERTY()
	TObjectPtr<USMHIC_TutorialAI> HIC;

	float CurrentHP;

	TWeakObjectPtr<AActor> LastAttacker;

	uint32 bIsNoteState:1 = false;
};
