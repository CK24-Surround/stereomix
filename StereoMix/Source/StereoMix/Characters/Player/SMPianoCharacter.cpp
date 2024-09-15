// Copyright Surround, Inc. All Rights Reserved.


#include "SMPianoCharacter.h"

#include "NiagaraComponent.h"
#include "Data/Character/SMPianoCharacterDataAsset.h"
#include "Utilities/SMLog.h"


ASMPianoCharacter::ASMPianoCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	ImpactArrowIndicatorNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ImpactArrowIndicatorNiagaraComponent"));
	ImpactArrowIndicatorNiagaraComponent->SetAbsolute(true, true, true);
	ImpactArrowIndicatorNiagaraComponent->SetAutoActivate(false);
}

void ASMPianoCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	const USMPianoCharacterDataAsset* PianoDataAsset = Cast<USMPianoCharacterDataAsset>(DataAsset);
	if (PianoDataAsset)
	{
		const TObjectPtr<UNiagaraSystem>* ImpactArrowIndicatorPtr = PianoDataAsset->ImpactArrowIndicator.Find(GetTeam());
		if (ImpactArrowIndicatorPtr)
		{
			UNiagaraSystem* ImpactArrowIndicator = *ImpactArrowIndicatorPtr;
			if (ImpactArrowIndicator)
			{
				ImpactArrowIndicatorNiagaraComponent->SetAsset(ImpactArrowIndicator);
			}
		}
	}
}

void ASMPianoCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ASMPianoCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	DrawDebugSphere(GetWorld(), GetMesh()->GetSocketLocation(TEXT("ik_hand_weapon_l")), 30.0f, 16, FColor::Emerald, false, 0.1f);
}
