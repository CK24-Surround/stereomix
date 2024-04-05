// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "StereoMixCharacter.generated.h"

UCLASS()
class STEREOMIX_API AStereoMixCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AStereoMixCharacter();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
