// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SMTeamComponentInterface.generated.h"

class USMTeamComponent;
// This class does not need to be modified.
UINTERFACE()
class USMTeamComponentInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class STEREOMIX_API ISMTeamComponentInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual USMTeamComponent* GetTeamComponent() const = 0;
};
