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

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	TObjectPtr<UNiagaraComponent> ImpactArrowIndicatorNiagaraComponent;
};
