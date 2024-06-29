// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMPlayerCharacter.h"

#include "SMPlayerCharacter_Piano.generated.h"

UCLASS()
class STEREOMIX_API ASMPlayerCharacter_Piano : public ASMPlayerCharacter
{
	GENERATED_BODY()

public:
	ASMPlayerCharacter_Piano();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
};
