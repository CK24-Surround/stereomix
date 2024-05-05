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
	/** 자신을 잡고 있는 액터를 반환합니다. 서버에서만 유효합니다. */
	virtual AActor* GetCatchingMeActor() = 0;

	/** 자신을 잡고 있는 액터를 할당합니다. 서버에서만 유효합니다. */
	virtual void SetCatchingMeActor(AActor* InCatchingMeActor) = 0;

public:
	/** 잡힐때 필요한 처리를 합니다. 서버에서만 유효합니다. */
	virtual bool OnCaught(AActor* TargetActor) = 0;

	/** 잡기가 풀릴때 필요한 처리를 합니다. 서버에서만 유효합니다. */
	virtual bool OnReleasedCatch(AActor* TargetActor) = 0;

public:
	/** 잡을 수 있는지 여부를 반환합니다. 서버에서만 유효합니다. */
	virtual bool IsCatchble(AActor* TargetActor) = 0;
};
