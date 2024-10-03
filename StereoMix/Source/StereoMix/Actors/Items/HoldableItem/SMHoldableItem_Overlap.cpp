// Copyright Studio Surround. All Rights Reserved.


#include "SMHoldableItem_Overlap.h"

#include "Components/SphereComponent.h"
#include "Components/Item/SMHIC_OverlapItem.h"
#include "Utilities/SMLog.h"

ASMHoldableItem_Overlap::ASMHoldableItem_Overlap(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<USMHIC_OverlapItem>(HICName))
{
	ColliderComponent->InitSphereRadius(65.0f);

	HIC->OnHoldedStateEntry.AddUObject(this, &ASMHoldableItem_Overlap::OnHoldedStateEntry);
}

void ASMHoldableItem_Overlap::ActivateItem(AActor* InActivator)
{
	Super::ActivateItem(InActivator);
}

void ASMHoldableItem_Overlap::OnHoldedStateEntry()
{
	ActivateItem(HIC->GetActorHoldingMe());

	Destroy();
}
