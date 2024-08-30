// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SMHoldInteractionInterface.generated.h"

class USMHoldInteractionComponent;

// This class does not need to be modified.
UINTERFACE()
class USMHoldInteractionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class STEREOMIX_API ISMHoldInteractionInterface
{
	GENERATED_BODY()

public:
	virtual USMHoldInteractionComponent* GetHoldInteractionComponent() = 0;
};
