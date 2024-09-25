// Copyright Studio Surround. All Rights Reserved.


#include "SMHoldableItemBase.h"

#include "Components/SphereComponent.h"
#include "HoldInteraction/SMHoldInteractionComponent.h"
#include "Utilities/SMCollision.h"


const FName ASMHoldableItemBase::HICName = TEXT("HIC");

ASMHoldableItemBase::ASMHoldableItemBase(const FObjectInitializer& ObjectInitializer)
{
	Super::SetReplicateMovement(true);

	ColliderComponent->SetCollisionProfileName(SMCollisionProfileName::HoldableItem);

	HIC = CreateDefaultSubobject<USMHoldInteractionComponent>(HICName);
}
