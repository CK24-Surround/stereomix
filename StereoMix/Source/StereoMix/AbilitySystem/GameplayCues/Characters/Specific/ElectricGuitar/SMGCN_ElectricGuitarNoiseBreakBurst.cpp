// Copyright Studio Surround. All Rights Reserved.


#include "SMGCN_ElectricGuitarNoiseBreakBurst.h"

#include "NiagaraFunctionLibrary.h"
#include "Actors/Character/Player/SMPlayerCharacterBase.h"
#include "FunctionLibraries/SMGameplayCueBlueprintLibrary.h"

bool USMGCN_ElectricGuitarNoiseBreakBurst::OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const
{
	ASMPlayerCharacterBase* SourceCharacter = Cast<ASMPlayerCharacterBase>(MyTarget);
	if (!SourceCharacter)
	{
		return false;
	}

	FVector SourceLocation;
	FRotator SourceToTargetRotation;
	USMGameplayCueBlueprintLibrary::GetLocationAndRotation(Parameters, SourceLocation, SourceToTargetRotation);

	const float TotalDistance = Parameters.RawMagnitude;
	constexpr float MinStepDistance = 200.0f;
	const int StepCount = FMath::Max(1, TotalDistance / MinStepDistance); // 제로 디비전 방지
	const float DistancePerStep = TotalDistance / StepCount;
	const float InitialOffset = DistancePerStep / 2.0f;

	TWeakObjectPtr<ASMPlayerCharacterBase> SourceCharacterWeakPtr = MakeWeakObjectPtr(SourceCharacter);
	const ESMTeam SourceTeam = SourceCharacter->GetTeam();
	UNiagaraSystem* CachedVFX = Parameters.AbilityLevel == 0 ? (VFX.Contains(SourceTeam) ? VFX[SourceTeam] : nullptr) : HealPackVFX;
	for (int32 i = 0; i < StepCount; ++i)
	{
		const float CurrentStepDistance = InitialOffset + (DistancePerStep * i);
		const FVector CurrentStepLocation = SourceLocation + (SourceToTargetRotation.Vector() * CurrentStepDistance);

		auto SpawnFX = [SourceCharacterWeakPtr, CachedVFX, CurrentStepLocation, SourceToTargetRotation]() {
			if (SourceCharacterWeakPtr.IsValid() && CachedVFX)
			{
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(SourceCharacterWeakPtr.Get(), CachedVFX, CurrentStepLocation, SourceToTargetRotation, FVector(1), false, true, ENCPoolMethod::AutoRelease);
			}
		};

		if (i == 0)
		{
			SpawnFX();
		}
		else
		{
			FTimerHandle TimerHandle;
			SourceCharacter->GetWorld()->GetTimerManager().SetTimer(TimerHandle, SpawnFX, i * 0.016f, false);
		}
	}

	return true;
}
