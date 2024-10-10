// Copyright Studio Surround. All Rights Reserved.


#include "SMGCN_Slash.h"

#include "FMODBlueprintStatics.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Actors/Character/Player/SMPlayerCharacterBase.h"

USMGCN_Slash::USMGCN_Slash()
{
	for (int32 i = 1; i < static_cast<int32>(ESMTeam::Max); ++i)
	{
		ESMTeam Key = static_cast<ESMTeam>(i);
		RightSFX.Add(Key, nullptr);
	}
}

bool USMGCN_Slash::OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const
{
	const ASMPlayerCharacterBase* SourceCharacter = Cast<ASMPlayerCharacterBase>(MyTarget);
	if (!SourceCharacter)
	{
		return false;
	}

	const ESMTeam SourceTeam = SourceCharacter->GetTeam();
	const bool bIsLeftSlash = FMath::IsNearlyZero(Parameters.RawMagnitude);
	USceneComponent* AttachToComponent = SourceCharacter->GetRootComponent();

	if (VFX.Contains(SourceTeam))
	{
		const FRotator ForwardRotation = SourceCharacter->GetActorRotation();
		const FRotator FinalRotation = bIsLeftSlash ? ForwardRotation : (ForwardRotation + FRotator(0.0, 0.0, 180.0));
		if (UNiagaraComponent* VFXComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(VFX[SourceTeam], AttachToComponent, NAME_None, FVector::ZeroVector, FinalRotation, EAttachLocation::KeepRelativeOffset, false, true, ENCPoolMethod::AutoRelease))
		{
			VFXComponent->SetAbsolute(false, true, true);
		}
	}

	if (SFX.Contains(SourceTeam) && RightSFX.Contains(SourceTeam))
	{
		UFMODEvent* SFXToPlay = bIsLeftSlash ? SFX[SourceTeam] : RightSFX[SourceTeam];
		UFMODBlueprintStatics::PlayEventAttached(SFXToPlay, AttachToComponent, NAME_None, FVector::ZeroVector, EAttachLocation::KeepRelativeOffset, false, true, true);
	}

	return true;
}
