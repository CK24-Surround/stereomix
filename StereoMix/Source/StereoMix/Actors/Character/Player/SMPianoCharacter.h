// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMPlayerCharacterBase.h"
#include "SMPianoCharacter.generated.h"

class UNiagaraComponent;

UCLASS()
class STEREOMIX_API ASMPianoCharacter : public ASMPlayerCharacterBase
{
	GENERATED_BODY()

public:
	ASMPianoCharacter(const FObjectInitializer& ObjectInitializer);

	void SetWeaponVFXEnabled(bool bNewIsEnabled);

protected:
	virtual void OnHoldStateEntry() override;

	virtual void OnHoldStateExit() override;
};
