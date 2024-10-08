// Copyright Studio Surround. All Rights Reserved.


#include "SMHoldableItem_Heal.h"

#include "StereoMixLog.h"
#include "Components/SphereComponent.h"
#include "Components/Item/SMHIC_HealItem.h"
#include "Utilities/SMCollision.h"


ASMHoldableItem_Heal::ASMHoldableItem_Heal(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<USMHIC_HealItem>(HICName))
{
	ColliderComponent->InitSphereRadius(65.0f);

	HIC->OnHeldStateEntry.AddUObject(this, &ThisClass::OnHeldStateEntry);
	HIC->OnHeldStateExit.AddUObject(this, &ThisClass::OnHeldStateExit);
}

void ASMHoldableItem_Heal::ActivateItemByNoiseBreak(const UWorld* World, const TArray<ASMTile*>& TilesToBeCaptured, AActor* InActivator, const TOptional<ESMTeam>& TeamOption)
{
	Super::ActivateItemByNoiseBreak(World, TilesToBeCaptured, InActivator, TeamOption);

	UE_LOG(LogStereoMix, Warning, TEXT("ASMHoldableItem_Heal::ActivateItemByNoiseBreak"));

	ActivateItem(InActivator);
}

void ASMHoldableItem_Heal::OnHeldStateEntry() const
{
	ColliderComponent->SetCollisionProfileName(SMCollisionProfileName::NoCollision);
	MeshComponent->SetVisibility(false);
}

void ASMHoldableItem_Heal::OnHeldStateExit() const
{
	ColliderComponent->SetCollisionProfileName(SMCollisionProfileName::HoldableItem);
	MeshComponent->SetVisibility(true);
}
