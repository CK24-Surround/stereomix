// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Character/SMCharacterBase.h"
#include "Interfaces/SMDamageInterface.h"
#include "SMAICharacterBase.generated.h"

UCLASS()
class STEREOMIX_API ASMAICharacterBase : public ASMCharacterBase, public ISMDamageInterface
{
	GENERATED_BODY()

public:
	ASMAICharacterBase();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "HitBox")
	TObjectPtr<UCapsuleComponent> HitBox;

public:
	virtual AActor* GetLastAttacker() const override;
	
	virtual void SetLastAttacker(AActor* NewAttacker) override;
	
	virtual void ReceiveDamage(AActor* NewAttacker, float InDamageAmount) override;
	
	virtual bool CanIgnoreAttack() const override { return false; }
	
	virtual bool IsObstacle() override { return false; }

	virtual void Tick(float DeltaTime) override;
};
