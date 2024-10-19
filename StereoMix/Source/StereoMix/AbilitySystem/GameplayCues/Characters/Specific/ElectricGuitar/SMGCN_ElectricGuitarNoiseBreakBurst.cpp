// Copyright Studio Surround. All Rights Reserved.


#include "SMGCN_ElectricGuitarNoiseBreakBurst.h"

#include "FMODBlueprintStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "Actors/Character/Player/SMPlayerCharacterBase.h"
#include "FunctionLibraries/SMGameplayCueBlueprintLibrary.h"

bool USMGCN_ElectricGuitarNoiseBreakBurst::OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const
{
	ASMPlayerCharacterBase* OwnerCharacter = Cast<ASMPlayerCharacterBase>(MyTarget);
	if (!OwnerCharacter)
	{
		return false;
	}

	FVector StartLocation;
	FRotator StartToTargetRotation;
	USMGameplayCueBlueprintLibrary::GetLocationAndRotation(Parameters, StartLocation, StartToTargetRotation);

	const float TotalDistance = Parameters.RawMagnitude;
	constexpr float MinStepDistance = 200.0f;
	const int StepCount = FMath::Max(1, TotalDistance / MinStepDistance); // 제로 디비전 방지
	const float DistancePerStep = TotalDistance / StepCount;
	const float InitialOffset = DistancePerStep / 2.0f;

	TWeakObjectPtr<ASMPlayerCharacterBase> SourceCharacterWeakPtr = MakeWeakObjectPtr(OwnerCharacter);
	const ESMTeam SourceTeam = OwnerCharacter->GetTeam();
	UNiagaraSystem* CachedVFX = Parameters.AbilityLevel == 0 ? (VFX.Contains(SourceTeam) ? VFX[SourceTeam] : nullptr) : HealPackVFX;
	for (int32 i = 0; i < StepCount; ++i)
	{
		const float CurrentStepDistance = InitialOffset + (DistancePerStep * i);
		const FVector CurrentStepLocation = StartLocation + (StartToTargetRotation.Vector() * CurrentStepDistance);

		auto SpawnFX = [SourceCharacterWeakPtr, CachedVFX, CurrentStepLocation, StartToTargetRotation]() {
			if (SourceCharacterWeakPtr.IsValid() && CachedVFX)
			{
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(SourceCharacterWeakPtr.Get(), CachedVFX, CurrentStepLocation, StartToTargetRotation, FVector(1), false, true, ENCPoolMethod::AutoRelease);
			}
		};

		if (i == 0)
		{
			SpawnFX();
		}
		else
		{
			FTimerHandle TimerHandle;
			OwnerCharacter->GetWorld()->GetTimerManager().SetTimer(TimerHandle, SpawnFX, i * DelayStep, false);
		}
	}

	const auto OwnerTeam = OwnerCharacter->GetTeam();
	if (UFMODEvent* CachedSFX = SFX.Contains(OwnerTeam) ? SFX[OwnerTeam] : nullptr)
	{
		UFMODBlueprintStatics::PlayEventAtLocation(SourceCharacterWeakPtr.Get(), CachedSFX, FTransform(StartLocation), true);
	}

	return true;
}
