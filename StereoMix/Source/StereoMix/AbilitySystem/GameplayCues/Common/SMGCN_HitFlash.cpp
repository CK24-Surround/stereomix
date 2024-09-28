// Copyright Studio Surround. All Rights Reserved.


#include "SMGCN_HitFlash.h"

#include "Utilities/SMLog.h"


bool USMGCN_HitFlash::OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const
{
	USkeletalMeshComponent* SourceMesh = Cast<USkeletalMeshComponent>(Parameters.TargetAttachComponent);
	UWorld* World = MyTarget ? MyTarget->GetWorld() : nullptr;
	if (!SourceMesh || !World)
	{
		return false;
	}

	NET_LOG(MyTarget, Warning, TEXT("섬광"));
	SourceMesh->SetCustomDepthStencilValue(1);

	TWeakObjectPtr<USkeletalMeshComponent> SourceMeshWeakPtr = MakeWeakObjectPtr(SourceMesh);
	auto RecoverStencilValue = [SourceMeshWeakPtr]() {
		if (SourceMeshWeakPtr.Get())
		{
			SourceMeshWeakPtr->SetCustomDepthStencilValue(0);
		}
	};

	FTimerHandle TimerHandle;
	World->GetTimerManager().SetTimer(TimerHandle, RecoverStencilValue, 0.1f, false);

	return false;
}
