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

	virtual void OnRep_PlayerState() override;

	UNiagaraComponent* GetImpactArrowIndicator() { return ImpactArrowIndicatorNiagaraComponent; }

	void SetWeaponVFXEnabled(bool bNewIsEnabled);

protected:
	virtual void OnHoldStateEntry() override;

	virtual void OnHoldStateExit() override;

	TObjectPtr<UNiagaraComponent> ImpactArrowIndicatorNiagaraComponent;
};
