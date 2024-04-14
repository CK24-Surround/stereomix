// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SMCharacter.generated.h"

class USMCharacterAssetData;

UCLASS()
class STEREOMIX_API ASMCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASMCharacter();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	UPROPERTY()
	TObjectPtr<const USMCharacterAssetData> AssetData;
};
