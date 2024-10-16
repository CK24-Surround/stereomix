// Copyright Studio Surround. All Rights Reserved.


#include "SMGCN_NoiseBreakBurst_Piano.h"

#include "FMODBlueprintStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "Actors/Character/Player/SMPlayerCharacterBase.h"
#include "FunctionLibraries/SMGameplayCueBlueprintLibrary.h"


bool USMGCN_NoiseBreakBurst_Piano::OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const
{
	ASMPlayerCharacterBase* SourceCharacter = Cast<ASMPlayerCharacterBase>(MyTarget);
	if (!SourceCharacter)
	{
		return false;
	}

	FVector FXLocation;
	FRotator FXRotation;
	USMGameplayCueBlueprintLibrary::GetLocationAndRotation(Parameters, FXLocation, FXRotation);

	const ESMTeam SourceTeam = SourceCharacter->GetTeam();

	if (FMath::IsNearlyZero(Parameters.RawMagnitude)) // 0이면 캐릭터 아니면 아이템입니다.
	{
		if (VFX.Contains(SourceTeam))
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(SourceCharacter, VFX[SourceTeam], FXLocation, FXRotation, FVector(1), false, true, ENCPoolMethod::AutoRelease);
		}
	}
	else
	{
		if (HealPackVFX)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(SourceCharacter, HealPackVFX, FXLocation, FXRotation, FVector(1), false, true, ENCPoolMethod::AutoRelease);
		}
	}

	if (SFX.Contains(SourceTeam))
	{
		UFMODBlueprintStatics::PlayEventAtLocation(SourceCharacter, SFX[SourceTeam], FTransform(FXLocation), true);
	}

	return true;
}
