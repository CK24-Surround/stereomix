// Copyright Studio Surround. All Rights Reserved.


#include "SMAN_FootStepSound.h"

#include "FMODBlueprintStatics.h"
#include "FunctionLibraries/SMTeamBlueprintLibrary.h"

void USMAN_FootStepSound::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (AActor* SourceOwner = MeshComp ? MeshComp->GetOwner() : nullptr)
	{
		const FName TeamParameterName = TEXT("Team");
		const float TeamParameter = USMTeamBlueprintLibrary::IsSameLocalTeam(SourceOwner) ? 0.0f : 1.0f;
		const FFMODEventInstance FMODEventInstance = UFMODBlueprintStatics::PlayEventAtLocation(MeshComp, Event, MeshComp->GetComponentTransform(), true);
		UFMODBlueprintStatics::EventInstanceSetParameter(FMODEventInstance, TeamParameterName, TeamParameter);
	}
	else // 에디터에서 듣기 위한 사운드 재생입니다.
	{
		UFMODBlueprintStatics::PlayEventAtLocation(MeshComp, Event, MeshComp->GetComponentTransform(), true);
	}
}
