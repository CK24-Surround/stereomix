// Copyright Surround, Inc. All Rights Reserved.


#include "SMPianoCharacter.h"

#include "NiagaraComponent.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "Actors/Weapons/SMBow.h"
#include "Data/Character/SMPianoCharacterDataAsset.h"
#include "Utilities/SMLog.h"


ASMPianoCharacter::ASMPianoCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	CharacterType = ESMCharacterType::Piano;

	ImpactArrowIndicatorNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ImpactArrowIndicatorNiagaraComponent"));
	ImpactArrowIndicatorNiagaraComponent->SetAbsolute(true, true, true);
	ImpactArrowIndicatorNiagaraComponent->SetAutoActivate(false);
}

void ASMPianoCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (const USMPianoCharacterDataAsset* PianoDataAsset = Cast<USMPianoCharacterDataAsset>(DataAsset))
	{
		const TObjectPtr<UNiagaraSystem>* ImpactArrowIndicatorPtr = PianoDataAsset->ImpactArrowIndicator.Find(GetTeam());
		if (UNiagaraSystem* ImpactArrowIndicator = ImpactArrowIndicatorPtr ? *ImpactArrowIndicatorPtr : nullptr)
		{
			ImpactArrowIndicatorNiagaraComponent->SetAsset(ImpactArrowIndicator);
		}
	}
}

void ASMPianoCharacter::SetWeaponVFXEnabled(bool bNewIsEnabled)
{
	if (ASMBow* Bow = GetWeapon<ASMBow>())
	{
		Bow->SetVFXEnabled(bNewIsEnabled);
	}
}

void ASMPianoCharacter::OnHoldStateEntry()
{
	Super::OnHoldStateEntry();

	if (HasAuthority())
	{
		if (UAbilitySystemComponent* SourceASC = GetAbilitySystemComponent<USMAbilitySystemComponent>())
		{
			FGameplayCueParameters GCParams;
			GCParams.SourceObject = this;
			SourceASC->AddGameplayCue(SMTags::GameplayCue::Piano::HoldWeapon, GCParams);
		}
	}
}

void ASMPianoCharacter::OnHoldStateExit()
{
	Super::OnHoldStateExit();

	if (HasAuthority())
	{
		if (UAbilitySystemComponent* SourceASC = GetAbilitySystemComponent<USMAbilitySystemComponent>())
		{
			SourceASC->RemoveGameplayCue(SMTags::GameplayCue::Piano::HoldWeapon);
		}
	}
}
