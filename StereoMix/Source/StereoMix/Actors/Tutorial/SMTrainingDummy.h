// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Actors/Character/SMCharacterBase.h"
#include "Interfaces/SMDamageInterface.h"
#include "Interfaces/SMHoldInteractionInterface.h"
#include "Interfaces/SMTeamInterface.h"
#include "SMTrainingDummy.generated.h"

class UNiagaraSystem;
class USMUserWidget_TrainingDummyState;
class UWidgetComponent;
class ASMNoteBase;
class UCapsuleComponent;
class USphereComponent;
class USMHIC_TrainingDummy;

DECLARE_DELEGATE(FOnTrainingDummyStateChanged);
DECLARE_DELEGATE_TwoParams(FOnTrainingDummyStatChanged, float /*Current*/, float /*Max*/);

UCLASS()
class STEREOMIX_API ASMTrainingDummy : public ASMCharacterBase, public ISMTeamInterface, public ISMDamageInterface, public ISMHoldInteractionInterface
{
	GENERATED_BODY()

public:
	ASMTrainingDummy();

	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void Tick(float DeltaTime) override;

	virtual void SetActorHiddenInGame(bool bNewHidden) override;

	virtual USMTeamComponent* GetTeamComponent() const override { return TeamComponent; }

	virtual ESMTeam GetTeam() const override;

	virtual AActor* GetLastAttacker() const override { return nullptr; }

	virtual void SetLastAttacker(AActor* NewAttacker) override {}

	virtual void ReceiveDamage(AActor* NewAttacker, float InDamageAmount) override;

	virtual bool CanIgnoreAttack() const override;

	virtual bool IsObstacle() override { return false; }

	virtual USMHoldInteractionComponent* GetHoldInteractionComponent() const override;

	void SetCurrentHP(float InCurrentHP);

	void SetInvincible(uint32 bNewIsInvincible) { bIsInvincible = bNewIsInvincible; }

	bool IsNeutralized() const { return bIsNeutralized; }

	void Revival();

	void SetNoteState(bool bNewIsNoteState);

	FOnTrainingDummyStateChanged OnHalfHPReached;

	FOnTrainingDummyStateChanged OnNeutralized;

	FOnTrainingDummyStatChanged OnHPChanged;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Collider")
	TObjectPtr<USphereComponent> ColliderComponent;

	UPROPERTY(VisibleAnywhere, Category = "Note")
	TObjectPtr<USceneComponent> NoteRootComponent;

	UPROPERTY(VisibleAnywhere, Category = "Widget")
	TObjectPtr<UWidgetComponent> StateWidgetComponent;

	UPROPERTY(VisibleAnywhere, Category = "Team")
	TObjectPtr<USMTeamComponent> TeamComponent;

	UPROPERTY(VisibleAnywhere, Category = "HIC")
	TObjectPtr<USMHIC_TrainingDummy> HIC;

	UPROPERTY(EditAnywhere, Category = "Design")
	TSubclassOf<ASMNoteBase> NoteClass;

	UPROPERTY()
	TObjectPtr<ASMNoteBase> Note;

	UPROPERTY(EditAnywhere, Category = "Design")
	TSubclassOf<ASMWeaponBase> WeaponClass;

	UPROPERTY(EditAnywhere, Category = "Design")
	TSubclassOf<USMUserWidget_TrainingDummyState> TrainingDummyStateWidgetClass;

	UPROPERTY(EditAnywhere, Category = "Design")
	TObjectPtr<UNiagaraSystem> SpawnVFX;

	UPROPERTY(EditAnywhere, Category = "Stat")
	float MaxHP = 100.0f;


	float HP = 0.0f;

	uint32 bIsInvincible:1 = false;

	uint32 bIsNeutralized:1 = false;
};
