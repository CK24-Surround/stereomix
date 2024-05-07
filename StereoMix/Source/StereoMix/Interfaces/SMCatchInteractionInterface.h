// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SMCatchInteractionInterface.generated.h"

class USMCatchInteractionComponent;

// This class does not need to be modified.
UINTERFACE()
class USMCatchInteractionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class STEREOMIX_API ISMCatchInteractionInterface
{
	GENERATED_BODY()

public:
	virtual USMCatchInteractionComponent* GetCatchInteractionComponent() = 0;
};
