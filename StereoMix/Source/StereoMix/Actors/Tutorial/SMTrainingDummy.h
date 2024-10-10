// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/SMDamageInterface.h"
#include "Interfaces/SMHoldInteractionInterface.h"
#include "Interfaces/SMTeamInterface.h"
#include "SMTrainingDummy.generated.h"

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

	virtual USMTeamComponent* GetTeamComponent() const override { return TeamComponent; }

	virtual ESMTeam GetTeam() const override;

	virtual AActor* GetLastAttacker() const override { return nullptr; }

	virtual void SetLastAttacker(AActor* NewAttacker) override {}

	virtual void ReceiveDamage(AActor* NewAttacker, float InDamageAmount) override;

	virtual bool CanIgnoreAttack() const override { return false; }

	virtual bool IsObstacle() override { return false; }

	virtual USMHoldInteractionComponent* GetHoldInteractionComponent() const override;

	FOnTrainingDummyStateChanged OnHalfHPReached;

	FOnTrainingDummyStateChanged OnNeutralized;

protected:
	UPROPERTY(EditAnywhere, Category = "Root")
	TObjectPtr<UCapsuleComponent> RootCapsuleComponent;

	UPROPERTY(EditAnywhere, Category = "Visual")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(EditAnywhere, Category = "Team")
	TObjectPtr<USMTeamComponent> TeamComponent;

	UPROPERTY(EditAnywhere, Category = "HIC")
	TObjectPtr<USMHIC_TrainingDummy> HIC;

	UPROPERTY(EditAnywhere, Category = "Stat")
	float MaxHP = 100.0f;

	float HP = 0.0f;
};
