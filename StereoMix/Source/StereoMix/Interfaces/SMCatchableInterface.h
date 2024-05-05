// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SMCatchableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class USMCatchableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class STEREOMIX_API ISMCatchableInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/** 자신이 잡고 있는 액터를 반환합니다. */
	virtual AActor* GetCatchActor() = 0;

	/** 자신이 잡고 있는 액터를 할당합니다. */
	virtual void SetCatchActor(AActor* CaughtActor) = 0;

	/** 자신을 잡고 있는 액터를 반환합니다. */
	virtual AActor* GetCaughtActor() = 0;

	/** 자신을 잡고 있는 액터를 할당합니다. */
	virtual void SetCaughtActor() = 0;
};
