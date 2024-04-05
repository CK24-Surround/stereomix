// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "StereoMixCharacter.h"
#include "StereoMixPlayerCharacter.generated.h"

class UGameplayAbility;
class AStereoMixPlayerController;
struct FInputActionValue;
class USphereComponent;
class UCameraComponent;
class USpringArmComponent;

UCLASS()
class STEREOMIX_API AStereoMixPlayerCharacter : public AStereoMixCharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AStereoMixPlayerCharacter();

protected:
	virtual void PostInitializeComponents() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_Controller() override;
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual void OnRep_PlayerState() override;

protected:
	void InitCamera();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Component|HitBox")
	TObjectPtr<USphereComponent> HitBox;
	
	UPROPERTY(VisibleAnywhere, Category = "Component|Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = "Component|Camera")
	TObjectPtr<UCameraComponent> Camera;

// ~Caching Section
protected:
	UPROPERTY()
	TObjectPtr<AStereoMixPlayerController> CachedStereoMixPlayerController;
// ~Caching Section

// ~GAS Section
protected:
	UPROPERTY()
	TSoftObjectPtr<UAbilitySystemComponent> ASC;

	UPROPERTY(EditDefaultsOnly, Category = "GAS|GA")
	TMap<int32, UGameplayAbility*> DefaultActiveAbilities;

	UPROPERTY(EditDefaultsOnly, Category = "GAS|GA")
	TMap<int32, UGameplayAbility*> DefaultAbilities;
// ~GAS Section
	
// ~Movement Section
protected:
	void Move(const FInputActionValue& InputActionValue);

	FVector GetCursorTargetingPoint();
// ~Movement Section

	
};
