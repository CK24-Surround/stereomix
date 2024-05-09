// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Data/SMTeam.h"
#include "SMTeamInterface.generated.h"

class USMTeamComponent;
// This class does not need to be modified.
UINTERFACE()
class USMTeamInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class STEREOMIX_API ISMTeamInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/** 팀 컴포넌트를 반환합니다. */
	virtual USMTeamComponent* GetTeamComponent() const = 0;

	/** 팀을 반환합니다. */
	virtual ESMTeam GetTeam() const = 0;
};
