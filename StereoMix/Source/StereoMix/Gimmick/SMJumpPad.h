// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "SMJumpPad.generated.h"

UCLASS()
class STEREOMIX_API ASMJumpPad : public AActor
{
	GENERATED_BODY()

public:
	ASMJumpPad();

protected:
	void Jump(ACharacter* InCharacter, const FVector& TargetLocation);
};
