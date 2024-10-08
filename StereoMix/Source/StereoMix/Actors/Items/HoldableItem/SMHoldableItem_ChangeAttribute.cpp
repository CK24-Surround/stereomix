// Copyright Studio Surround. All Rights Reserved.


#include "SMHoldableItem_ChangeAttribute.h"

#include "StereoMixLog.h"
#include "Actors/Character/Player/SMPlayerCharacterBase.h"
#include "Components/SphereComponent.h"
#include "Components/Item/SMHIC_HealItem.h"
#include "Utilities/SMCollision.h"


ASMHoldableItem_ChangeAttribute::ASMHoldableItem_ChangeAttribute(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<USMHIC_HealItem>(HICName))
{
	ColliderComponent->InitSphereRadius(65.0f);

	HIC->OnHeldStateEntry.AddUObject(this, &ThisClass::OnHeldStateEntry);
}

void ASMHoldableItem_ChangeAttribute::ActivateItemByNoiseBreak(const UWorld* World, const TArray<ASMTile*>& TilesToBeCaptured, AActor* InActivator, const TOptional<ESMTeam>& TeamOption)
{
	Super::ActivateItemByNoiseBreak(World, TilesToBeCaptured, InActivator, TeamOption);

	UE_LOG(LogStereoMix, Warning, TEXT("ASMHoldableItem_Heal::ActivateItemByNoiseBreak"));

	ActivateItem(InActivator);
	Destroy();
}

void ASMHoldableItem_ChangeAttribute::OnHeldStateEntry()
{
	ColliderComponent->SetCollisionProfileName(SMCollisionProfileName::NoCollision);
	MeshComponent->SetVisibility(false);
	if (ASMPlayerCharacterBase* HoldingMePlayer = Cast<ASMPlayerCharacterBase>(HIC->GetActorHoldingMe()))
	{
		HoldingMePlayer->GetHoldInteractionComponent()->OnHeldStateExit.AddUObject(this, &ThisClass::OnHeldStateExit);
	}
}

void ASMHoldableItem_ChangeAttribute::OnHeldStateExit()
{
	ActivateItem(nullptr);
	Destroy();
}
