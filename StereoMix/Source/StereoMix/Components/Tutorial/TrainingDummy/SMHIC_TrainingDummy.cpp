// Copyright Studio Surround. All Rights Reserved.


#include "SMHIC_TrainingDummy.h"

#include "Actors/Character/Player/SMBassCharacter.h"
#include "Actors/Character/Player/SMElectricGuitarCharacter.h"
#include "Actors/Character/Player/SMPianoCharacter.h"
#include "Actors/Tutorial/SMTrainingDummy.h"
#include "Components/CapsuleComponent.h"


USMHIC_TrainingDummy::USMHIC_TrainingDummy()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool USMHIC_TrainingDummy::CanBeHeld(AActor* Instigator) const
{
	const ASMTrainingDummy* SourceCharacter = GetOwner<ASMTrainingDummy>();
	if (SourceCharacter && SourceCharacter->IsNeutralized())
	{
		return true;
	}

	return false;
}

void USMHIC_TrainingDummy::OnHeld(AActor* Instigator)
{
	if (ASMTrainingDummy* SourceCharacter = GetOwner<ASMTrainingDummy>())
	{
		SourceCharacter->SetActorHiddenInGame(true);
	}
}

void USMHIC_TrainingDummy::OnReleasedFromHold(AActor* Instigator)
{
	ReleasedFromHold();
}

void USMHIC_TrainingDummy::OnNoiseBreakStarted(AActor* Instigator) {}

void USMHIC_TrainingDummy::OnNoiseBreakApplied(ASMElectricGuitarCharacter* Instigator, TSharedPtr<FSMNoiseBreakData> NoiseBreakData)
{
	InternalOnNoiseBreakApplied(Instigator, NoiseBreakData);
}

void USMHIC_TrainingDummy::OnNoiseBreakApplied(ASMPianoCharacter* Instigator, TSharedPtr<FSMNoiseBreakData> NoiseBreakData)
{
	InternalOnNoiseBreakApplied(Instigator, NoiseBreakData);
}

void USMHIC_TrainingDummy::OnNoiseBreakApplied(ASMBassCharacter* Instigator, TSharedPtr<FSMNoiseBreakData> NoiseBreakData)
{
	InternalOnNoiseBreakApplied(Instigator, NoiseBreakData);
}

void USMHIC_TrainingDummy::ReleasedFromHold()
{
	if (ASMTrainingDummy* SourceCharacter = GetOwner<ASMTrainingDummy>())
	{
		SourceCharacter->SetActorHiddenInGame(false);
	}

	UE_LOG(LogTemp, Warning, TEXT("놓기"));
}

void USMHIC_TrainingDummy::InternalOnNoiseBreakApplied(ASMPlayerCharacterBase* Instigator, const TSharedPtr<FSMNoiseBreakData>& NoiseBreakData)
{
	ASMTrainingDummy* SourceCharacter = GetOwner<ASMTrainingDummy>();
	if (!SourceCharacter || !NoiseBreakData)
	{
		return;
	}

	if (const UCapsuleComponent* SourceCapsule = SourceCharacter->GetComponentByClass<UCapsuleComponent>())
	{
		const FVector Offset(0.0, 0.0, SourceCapsule->GetScaledCapsuleHalfHeight());
		const FVector NewLocation = NoiseBreakData->NoiseBreakLocation + Offset;
		SourceCharacter->SetActorLocation(NewLocation);
		SourceCharacter->Revival();
	}

	ReleasedFromHold();
}
