// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Data/Enum/SMOutlineStencil.h"
#include "SMCharacterBase.generated.h"

UCLASS(Abstract)
class STEREOMIX_API ASMCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	ASMCharacterBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	ESMShaderStencil GetDefaultShaderStencil() const { return DefaultShaderStencil; }

protected:
	ESMShaderStencil DefaultShaderStencil = ESMShaderStencil::NonOutline;
};
