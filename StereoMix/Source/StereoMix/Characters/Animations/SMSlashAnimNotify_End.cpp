// Copyright Surround, Inc. All Rights Reserved.


#include "SMSlashAnimNotify_End.h"

#include "Characters/SMBassCharacter.h"
#include "Utilities/SMLog.h"


FString USMSlashAnimNotify_End::GetNotifyName_Implementation() const
{
	return TEXT("SMSlashAnimNotifyEnd");
}

void USMSlashAnimNotify_End::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp)
	{
		return;
	}

	ASMBassCharacter* SourceCharacter = Cast<ASMBassCharacter>(MeshComp->GetOwner());
	if (SourceCharacter)
	{
		NET_LOG(SourceCharacter, Warning, TEXT("베기 종료"));
	}
}
