// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Character/SMCharacterBase.h"
#include "Interfaces/SMDamageInterface.h"
#include "Interfaces/SMHoldInteractionInterface.h"
#include "Interfaces/SMTeamInterface.h"
#include "SMAICharacterBase.generated.h"

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

	virtual void Tick(float DeltaTime) override;

	virtual USMHoldInteractionComponent* GetHoldInteractionComponent() const override;
	
	virtual USMTeamComponent* GetTeamComponent() const override { return TeamComponent; }

	virtual ESMTeam GetTeam() const override;

	virtual AActor* GetLastAttacker() const override;

	virtual void SetLastAttacker(AActor* NewAttacker) override;

	virtual void ReceiveDamage(AActor* NewAttacker, float InDamageAmount) override;

	virtual bool CanIgnoreAttack() const override { return false; }

	virtual bool IsObstacle() override { return false; }

	UFUNCTION(BlueprintCallable, Category = "Note")
	bool IsNoteState() const { return bIsNoteState; }

	void SetNoteState(bool bNewIsNote);

	ASMNoteBase* GetNote() const { return Note; }
	
protected:
	UPROPERTY(VisibleAnywhere, Category = "HitBox")
	TObjectPtr<UCapsuleComponent> HitBox;

	UPROPERTY(VisibleAnywhere, Category = "Team")
	TObjectPtr<USMTeamComponent> TeamComponent;

	UPROPERTY(VisibleAnywhere, Category = "Note")
	TObjectPtr<USceneComponent> NoteSlotComponent;

	UPROPERTY(EditAnywhere, Category = "Design|Weapon")
	TSubclassOf<ASMWeaponBase> WeaponClass;

	UPROPERTY(EditAnywhere, Category = "Design|Weapon")
	FName WeaponSocketName;

	UPROPERTY(EditAnywhere, Category = "Design|Note")
	TSubclassOf<ASMNoteBase> NoteClass;

	UPROPERTY()
	TObjectPtr<ASMWeaponBase> Weapon;

	UPROPERTY()
	TObjectPtr<ASMNoteBase> Note;

	UPROPERTY()
	TObjectPtr<USMHIC_TutorialAI> HIC;

	uint32 bIsNoteState:1 = false;
};
