// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "SMDamageInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class USMDamageInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class STEREOMIX_API ISMDamageInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/** 마지막으로 공격한 가해자를 가져옵니다. */
	virtual AActor* GetLastAttackInstigator() = 0;

	/** 마지막으로 공격한 가해자를 저장할때 사용합니다. */
	virtual void SetLastAttackInstigator(AActor* NewStunInstigator) = 0;
};
