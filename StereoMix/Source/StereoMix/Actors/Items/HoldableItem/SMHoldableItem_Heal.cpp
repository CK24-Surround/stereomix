// Copyright Studio Surround. All Rights Reserved.


#include "SMHoldableItem_Heal.h"

#include "Components/SphereComponent.h"
#include "Components/Item/SMHIC_HealItem.h"


ASMHoldableItem_Heal::ASMHoldableItem_Heal(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<USMHIC_HealItem>(HICName))
{
	ColliderComponent->InitSphereRadius(65.0f);

	HIC->OnHoldedStateEntry.AddUObject(this, &ASMHoldableItem_Heal::OnHeldStateEntry);
}

void ASMHoldableItem_Heal::ActivateItem(AActor* InActivator)
{
	Super::ActivateItem(InActivator);
}

void ASMHoldableItem_Heal::OnHeldStateEntry()
{
	ColliderComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComponent->SetVisibility(false);
}
