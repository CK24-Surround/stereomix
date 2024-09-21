// Copyright Studio Surround. All Rights Reserved.


#include "SMHoldableItemBase.h"

#include "HoldInteraction/SMHoldInteractionComponent.h"

const FName ASMHoldableItemBase::HICName = TEXT("HIC");

ASMHoldableItemBase::ASMHoldableItemBase(const FObjectInitializer& ObjectInitializer)
{
	HIC = CreateDefaultSubobject<USMHoldInteractionComponent>(HICName);

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(RootComponent);
}
