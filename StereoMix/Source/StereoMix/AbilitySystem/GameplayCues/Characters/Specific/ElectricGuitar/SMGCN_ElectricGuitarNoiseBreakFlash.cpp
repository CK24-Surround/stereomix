// Copyright Studio Surround. All Rights Reserved.


#include "SMGCN_ElectricGuitarNoiseBreakFlash.h"

#include "FMODBlueprintStatics.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Actors/Character/Player/SMPlayerCharacterBase.h"
#include "FunctionLibraries/SMGameplayCueBlueprintLibrary.h"

bool USMGCN_ElectricGuitarNoiseBreakFlash::OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const
{
	ASMPlayerCharacterBase* SourceCharacter = Cast<ASMPlayerCharacterBase>(MyTarget);
	if (!SourceCharacter)
	{
		return false;
	}

	FVector StartLocation;
	FRotator StartRotation;
	USMGameplayCueBlueprintLibrary::GetLocationAndRotation(Parameters, StartLocation, StartRotation);

	const ESMTeam SourceTeam = SourceCharacter->GetTeam();

	if (VFX.Contains(SourceTeam))
	{
		if (UNiagaraComponent* VFXComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(SourceCharacter, VFX[SourceTeam], StartLocation, StartRotation, FVector(1), false, true, ENCPoolMethod::AutoRelease))
		{
			const FName LengthParameterName = TEXT("Length");
			const float LengthParameterValue = Parameters.RawMagnitude;
			VFXComponent->SetFloatParameter(LengthParameterName, LengthParameterValue);

			const FName DelayParameterName = TEXT("Delay");
			constexpr float DelayParameterValue = 0.5f;
			VFXComponent->SetFloatParameter(DelayParameterName, DelayParameterValue);
		}
	}

	if (SFX.Contains(SourceTeam))
	{
		UFMODBlueprintStatics::PlayEventAtLocation(SourceCharacter, SFX[SourceTeam], FTransform(StartLocation), true);
	}

	return true;
}
