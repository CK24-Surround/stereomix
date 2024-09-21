// Copyright Studio Surround. All Rights Reserved.


#include "SMHoldableItem_Overlap.h"

#include "Component/SMHIC_OverlapItem.h"
#include "Utilities/SMLog.h"

ASMHoldableItem_Overlap::ASMHoldableItem_Overlap(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<USMHIC_OverlapItem>(HICName))
{
	HIC->OnHoldedStateEntry.AddUObject(this, &ASMHoldableItem_Overlap::OnHoldedStateEntry);
}

void ASMHoldableItem_Overlap::ActivateItem(AActor* InActivator)
{
	NET_LOG(InActivator, Warning, TEXT("ASMHoldableItem_Overlap::ActivateItem"));
	
	Super::ActivateItem(InActivator);
}

void ASMHoldableItem_Overlap::OnHoldedStateEntry()
{
	ActivateItem(HIC->GetActorHoldingMe());
	Destroy();
}
