// Copyright Studio Surround. All Rights Reserved.


#include "SMGCN_HitFlash.h"

#include "Actors/Character/SMCharacterBase.h"
#include "Utilities/SMLog.h"


bool USMGCN_HitFlash::OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const
{
	UMeshComponent* SourceMesh = Cast<UMeshComponent>(Parameters.TargetAttachComponent);
	const UWorld* World = MyTarget ? MyTarget->GetWorld() : nullptr;
	if (!SourceMesh || !World)
	{
		return false;
	}

	const ASMCharacterBase* OwnerCharacter = Cast<ASMCharacterBase>(MyTarget);
	ESMShaderStencil DefaultShaderStencil = OwnerCharacter ? OwnerCharacter->GetDefaultShaderStencil() : ESMShaderStencil::NonOutline;

	SourceMesh->SetCustomDepthStencilValue(static_cast<int32>(ESMShaderStencil::Hit));

	auto RecoverStencilValue = [SourceMeshWeakPtr = MakeWeakObjectPtr(SourceMesh), CopiedDefaultShaderStencil = static_cast<int32>(DefaultShaderStencil)]() {
		if (SourceMeshWeakPtr.IsValid())
		{
			SourceMeshWeakPtr->SetCustomDepthStencilValue(CopiedDefaultShaderStencil);
		}
	};

	FTimerHandle TimerHandle;
	World->GetTimerManager().SetTimer(TimerHandle, RecoverStencilValue, 0.1f, false);

	return false;
}
