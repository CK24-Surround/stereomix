// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Character/SMCharacterBase.h"
#include "Interfaces/SMDamageInterface.h"
#include "Interfaces/SMTeamInterface.h"
#include "SMAICharacterBase.generated.h"

class USMPlayerCharacterDataAsset;
class ASMWeaponBase;

UCLASS()
class STEREOMIX_API ASMAICharacterBase : public ASMCharacterBase, public ISMTeamInterface, public ISMDamageInterface
{
	GENERATED_BODY()

public:
	ASMAICharacterBase();

	virtual void PostInitializeComponents() override;

	virtual USMTeamComponent* GetTeamComponent() const override { return TeamComponent; }

	virtual ESMTeam GetTeam() const override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "HitBox")
	TObjectPtr<UCapsuleComponent> HitBox;

	UPROPERTY(VisibleAnywhere, Category = "Team")
	TObjectPtr<USMTeamComponent> TeamComponent;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	TSubclassOf<ASMWeaponBase> WeaponClass;
	
	UPROPERTY(EditAnywhere, Category = "Weapon")
	FName WeaponSocketName;

	UPROPERTY()
	TObjectPtr<ASMWeaponBase> Weapon;

public:
	virtual AActor* GetLastAttacker() const override;

	virtual void SetLastAttacker(AActor* NewAttacker) override;

	virtual void ReceiveDamage(AActor* NewAttacker, float InDamageAmount) override;

	virtual bool CanIgnoreAttack() const override { return false; }

	virtual bool IsObstacle() override { return false; }

	virtual void Tick(float DeltaTime) override;
};
