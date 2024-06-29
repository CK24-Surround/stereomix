// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMPlayerCharacter.h"

#include "SMPlayerCharacter_Bass.generated.h"

UCLASS()
class STEREOMIX_API ASMPlayerCharacter_Bass : public ASMPlayerCharacter
{
	GENERATED_BODY()

public:
	ASMPlayerCharacter_Bass();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
};
