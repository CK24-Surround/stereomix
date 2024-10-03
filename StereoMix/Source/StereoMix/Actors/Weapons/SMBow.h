// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMWeaponBase.h"
#include "SMBow.generated.h"

class UNiagaraComponent;

UCLASS(Abstract)
class STEREOMIX_API ASMBow : public ASMWeaponBase
{
	GENERATED_BODY()

public:
	ASMBow(const FObjectInitializer& ObjectInitializer);

	void SetVFXEnabled(bool bNewIsVFXEnabled);

protected:
	UPROPERTY(VisibleAnywhere, Category = "Visual")
	TObjectPtr<UNiagaraComponent> VFXComponent;
};
