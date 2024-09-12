// Copyright Surround, Inc. All Rights Reserved.


#include "SMHoldInteractionBlueprintLibrary.h"

#include "Interfaces/SMHoldInteractionInterface.h"

USMHoldInteractionComponent* USMHoldInteractionBlueprintLibrary::GetHoldInteractionComponent(AActor* InActor)
{
	ISMHoldInteractionInterface* HII = Cast<ISMHoldInteractionInterface>(InActor);
	if (HII)
	{
		USMHoldInteractionComponent* HIC = HII->GetHoldInteractionComponent();
		if (ensureAlways(HIC))
		{
			return HIC;
		}
	}

	return nullptr;
}
