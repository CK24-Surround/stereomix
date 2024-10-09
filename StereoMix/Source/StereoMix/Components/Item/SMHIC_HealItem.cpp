// Copyright Studio Surround. All Rights Reserved.


#include "SMHIC_HealItem.h"

#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "Actors/Items/HoldableItem/SMHoldableItem_ChangeAttribute.h"
#include "FunctionLibraries/SMAbilitySystemBlueprintLibrary.h"


void USMHIC_HealItem::BeginPlay()
{
	Super::BeginPlay();

	SourceItem = GetOwner<ASMHoldableItem_ChangeAttribute>();
	OnHeldStateEntry.AddUObject(this, &ThisClass::OnHeldStateEntryCallback);
}

bool USMHIC_HealItem::CanBeHeld(AActor* Instigator) const
{
	if (!ensureAlways(Instigator))
	{
		return false;
	}

	return true;
}

void USMHIC_HealItem::OnHeld(AActor* Instigator)
{
	if (!ensureAlways(Instigator))
	{
		return;
	}

	SetActorHoldingMe(Instigator);
}

void USMHIC_HealItem::OnReleasedFromHold(AActor* Instigator)
{
	if (SourceItem && !SourceItem->GetIsActivated())
	{
		SourceItem->Destroy();
	}
}

void USMHIC_HealItem::OnNoiseBreakApplied(ASMElectricGuitarCharacter* Instigator, TSharedPtr<FSMNoiseBreakData> NoiseBreakData)
{
	TArray<TWeakObjectPtr<ASMTile>> TilesToBeCaptured = NoiseBreakData ? NoiseBreakData->TilesToBeTriggered : TArray<TWeakObjectPtr<ASMTile>>();
	InternalNoiseBreakApplied(Instigator, TilesToBeCaptured);
}

void USMHIC_HealItem::OnNoiseBreakApplied(ASMPianoCharacter* Instigator, TSharedPtr<FSMNoiseBreakData> NoiseBreakData)
{
	TArray<TWeakObjectPtr<ASMTile>> TilesToBeCaptured = NoiseBreakData ? NoiseBreakData->TilesToBeTriggered : TArray<TWeakObjectPtr<ASMTile>>();
	InternalNoiseBreakApplied(Instigator, TilesToBeCaptured);
}

void USMHIC_HealItem::OnNoiseBreakApplied(ASMBassCharacter* Instigator, TSharedPtr<FSMNoiseBreakData> NoiseBreakData)
{
	TArray<TWeakObjectPtr<ASMTile>> TilesToBeCaptured = NoiseBreakData ? NoiseBreakData->TilesToBeTriggered : TArray<TWeakObjectPtr<ASMTile>>();
	InternalNoiseBreakApplied(Instigator, TilesToBeCaptured);
}

void USMHIC_HealItem::InternalNoiseBreakApplied(AActor* InActivator, const TArray<TWeakObjectPtr<ASMTile>>& TilesToBeCaptured)
{
	if (!SourceItem)
	{
		return;
	}

	SourceItem->ActivateItemByNoiseBreak(InActivator, TilesToBeCaptured);
}

void USMHIC_HealItem::OnHeldStateEntryCallback()
{
	if (SourceItem)
	{
		SourceItem->SetActorHiddenInGame(true);
		SourceItem->SetActorEnableCollision(false);
	}
}
