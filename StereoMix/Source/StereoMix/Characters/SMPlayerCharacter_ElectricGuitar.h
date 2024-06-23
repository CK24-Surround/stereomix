// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMPlayerCharacter.h"
#include "SMPlayerCharacter_ElectricGuitar.generated.h"

UCLASS()
class STEREOMIX_API ASMPlayerCharacter_ElectricGuitar : public ASMPlayerCharacter
{
	GENERATED_BODY()

public:
	ASMPlayerCharacter_ElectricGuitar();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
};