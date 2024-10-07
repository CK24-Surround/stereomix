// Copyright Surround, Inc. All Rights Reserved.


#include "SMElectricGuitarCharacter.h"

#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "Actors/Weapons/SMWeaponBase.h"


ASMElectricGuitarCharacter::ASMElectricGuitarCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CharacterType = ESMCharacterType::ElectricGuitar;
}

void ASMElectricGuitarCharacter::OnHoldStateEntry()
{
	Super::OnHoldStateEntry();

	if (HasAuthority())
	{
		if (UAbilitySystemComponent* SourceASC = GetAbilitySystemComponent<USMAbilitySystemComponent>())
		{
			FGameplayCueParameters GCParams;
			GCParams.SourceObject = this;
			SourceASC->AddGameplayCue(SMTags::GameplayCue::ElectricGuitar::HoldWeapon, GCParams);
		}
	}
}

void ASMElectricGuitarCharacter::OnHoldStateExit()
{
	Super::OnHoldStateExit();

	if (HasAuthority())
	{
		if (UAbilitySystemComponent* SourceASC = GetAbilitySystemComponent<USMAbilitySystemComponent>())
		{
			SourceASC->RemoveGameplayCue(SMTags::GameplayCue::ElectricGuitar::HoldWeapon);
		}
	}
}
