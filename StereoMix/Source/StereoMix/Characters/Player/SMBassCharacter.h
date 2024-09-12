// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMPlayerCharacterBase.h"
#include "SMBassCharacter.generated.h"

class UBoxComponent;
class USMSlashComponent;

DECLARE_DELEGATE(FOnSlashSignature);

UCLASS(Abstract)
class STEREOMIX_API ASMBassCharacter : public ASMPlayerCharacterBase
{
	GENERATED_BODY()

public:
	ASMBassCharacter(const FObjectInitializer& ObjectInitializer);

	USceneComponent* GetSlashColliderRootComponent() const { return SlashColliderRootComponent; }

	UCapsuleComponent* GetSlashColliderComponent() const { return SlashColliderComponent; }

	UBoxComponent* GetChargeColliderComponent() const { return ChargeColliderComponent; }

protected:
	UPROPERTY(VisibleAnywhere, Category = "Collider")
	TObjectPtr<USceneComponent> SlashColliderRootComponent;

	UPROPERTY(VisibleAnywhere, Category = "Collider")
	TObjectPtr<UCapsuleComponent> SlashColliderComponent;

	UPROPERTY(VisibleAnywhere, Category = "Collider")
	TObjectPtr<UBoxComponent> ChargeColliderComponent;
};
