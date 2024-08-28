// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMPlayerCharacterBase.h"
#include "SMBassCharacter.generated.h"

class USMSlashComponent;
DECLARE_DELEGATE(FOnSlashSignature);

UCLASS(Abstract)
class STEREOMIX_API ASMBassCharacter : public ASMPlayerCharacterBase
{
	GENERATED_BODY()

public:
	ASMBassCharacter();

	virtual void PostInitializeComponents() override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	virtual void Tick(float DeltaSeconds) override;

	USMSlashComponent* GetSlashComponent() const { return SlashComponent; }

	USceneComponent* GetSlashColliderRootComponent() const { return SlashColliderRootComponent; }

	UCapsuleComponent* GetSlashColliderComponent() const { return SlashColliderComponent; }
	
	FOnSlashSignature OnSlash;

protected:
	void Slash();

	UPROPERTY(VisibleAnywhere, Category = "Slash")
	TObjectPtr<USMSlashComponent> SlashComponent;

	UPROPERTY(VisibleAnywhere, Category = "Collider")
	TObjectPtr<USceneComponent> SlashColliderRootComponent;

	UPROPERTY(VisibleAnywhere, Category = "Collider")
	TObjectPtr<UCapsuleComponent> SlashColliderComponent;
};
