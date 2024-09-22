// Copyright Studio Surround. All Rights Reserved.


#include "SMFunctionLibraryBase.h"

#include "GameInstance/SMGameInstance.h"

USMGameInstance* USMFunctionLibraryBase::GetGameInstance()
{
	if (!GEngine)
	{
		return nullptr;
	}

	USMGameInstance* GameInstance = nullptr;
	for (const FWorldContext& WorldContext : GEngine->GetWorldContexts())
	{
		if (USMGameInstance* CachedGameInstance = Cast<USMGameInstance>(WorldContext.OwningGameInstance))
		{
			GameInstance = CachedGameInstance;
			break;
		}
	}

	return GameInstance;
}
