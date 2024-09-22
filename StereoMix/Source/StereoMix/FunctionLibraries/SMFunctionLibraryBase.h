// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "SMFunctionLibraryBase.generated.h"

class USMGameInstance;

/**
 * 
 */
UCLASS()
class STEREOMIX_API USMFunctionLibraryBase : public UObject
{
	GENERATED_BODY()

protected:
	static USMGameInstance* GetGameInstance();
};
