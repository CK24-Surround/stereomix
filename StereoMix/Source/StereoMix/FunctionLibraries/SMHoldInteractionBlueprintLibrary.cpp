// Copyright Surround, Inc. All Rights Reserved.


#include "SMHoldInteractionBlueprintLibrary.h"

#include "Interfaces/SMHoldInteractionInterface.h"

USMHoldInteractionComponent* USMHoldInteractionBlueprintLibrary::GetHoldInteractionComponent(AActor* InActor)
{
	ISMHoldInteractionInterface* HIInterface = Cast<ISMHoldInteractionInterface>(InActor);
	if (ensureAlways(HIInterface))
	{
		USMHoldInteractionComponent* HIC = HIInterface->GetHoldInteractionComponent();
		if (ensureAlways(HIC))
		{
			return HIC;
		}
	}

	return nullptr;
}
