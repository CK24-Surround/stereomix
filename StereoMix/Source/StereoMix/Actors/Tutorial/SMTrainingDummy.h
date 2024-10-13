// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/SMDamageInterface.h"
#include "Interfaces/SMHoldInteractionInterface.h"
#include "Interfaces/SMTeamInterface.h"
#include "SMTrainingDummy.generated.h"

class ASMNoteBase;
class USphereComponent;
class USMHIC_TrainingDummy;
DECLARE_DELEGATE(FOnTrainingDummyStateChanged);

class UCapsuleComponent;

UCLASS()
class STEREOMIX_API ASMTrainingDummy : public AActor, public ISMTeamInterface, public ISMDamageInterface, public ISMHoldInteractionInterface
{
	GENERATED_BODY()

public:
	ASMTrainingDummy();

	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;

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

	void SetInvincible(uint32 bNewIsInvincible) { bIsInvincible = bNewIsInvincible; }

	bool IsNeutralized() const { return bIsNeutralized; }

	void Revival();

	void SetNoteState(bool bNewIsNoteState);

	FOnTrainingDummyStateChanged OnHalfHPReached;

	FOnTrainingDummyStateChanged OnNeutralized;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Root")
	TObjectPtr<UCapsuleComponent> RootCapsuleComponent;

	UPROPERTY(VisibleAnywhere, Category = "Collider")
	TObjectPtr<USphereComponent> ColliderComponent;

	UPROPERTY(VisibleAnywhere, Category = "Visual")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(VisibleAnywhere, Category = "Note")
	TObjectPtr<USceneComponent> NoteRootComponent;

	UPROPERTY(VisibleAnywhere, Category = "Team")
	TObjectPtr<USMTeamComponent> TeamComponent;

	UPROPERTY(VisibleAnywhere, Category = "HIC")
	TObjectPtr<USMHIC_TrainingDummy> HIC;

	UPROPERTY(EditAnywhere, Category = "Design")
	TSubclassOf<ASMNoteBase> NoteClass;

	UPROPERTY()
	TObjectPtr<ASMNoteBase> Note;

	UPROPERTY(EditAnywhere, Category = "Stat")
	float MaxHP = 100.0f;

	float HP = 0.0f;

	uint32 bIsInvincible:1 = false;

	uint32 bIsNeutralized:1 = false;
};
