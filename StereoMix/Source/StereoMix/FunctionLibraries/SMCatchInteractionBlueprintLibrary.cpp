// Copyright Surround, Inc. All Rights Reserved.


#include "SMCatchInteractionBlueprintLibrary.h"

#include "Interfaces/SMCatchInteractionInterface.h"

USMCatchInteractionComponent* USMCatchInteractionBlueprintLibrary::GetCatchInteractionComponent(AActor* InActor)
{
	ISMCatchInteractionInterface* CIInterface = Cast<ISMCatchInteractionInterface>(InActor);
	if (ensureAlways(CIInterface))
	{
		USMCatchInteractionComponent* CIC = CIInterface->GetCatchInteractionComponent();
		if (ensureAlways(CIC))
		{
			return CIC;
		}
	}

	return nullptr;
}
