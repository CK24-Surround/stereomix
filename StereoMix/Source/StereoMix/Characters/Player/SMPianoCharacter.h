// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMPlayerCharacterBase.h"
#include "SMPianoCharacter.generated.h"

UCLASS()
class STEREOMIX_API ASMPianoCharacter : public ASMPlayerCharacterBase
{
	GENERATED_BODY()

public:
	ASMPianoCharacter();

protected:
	virtual void BeginPlay() override;
};
