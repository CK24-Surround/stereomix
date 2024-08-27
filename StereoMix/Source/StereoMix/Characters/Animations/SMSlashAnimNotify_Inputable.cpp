// Copyright Surround, Inc. All Rights Reserved.


#include "SMSlashAnimNotify_Inputable.h"

#include "Characters/SMBassCharacter.h"
#include "Utilities/SMLog.h"

FString USMSlashAnimNotify_Inputable::GetNotifyName_Implementation() const
{
	return TEXT("SlashInputableNotify");
}

void USMSlashAnimNotify_Inputable::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp)
	{
		return;
	}

	ASMBassCharacter* SourceCharacter = Cast<ASMBassCharacter>(MeshComp->GetOwner());
	if (SourceCharacter)
	{
		if (!SourceCharacter->HasAuthority())
		{
			return;
		}

		if (bIsStart)
		{
			NET_LOG(SourceCharacter, Warning, TEXT("입력 가능 존 시작"))
			SourceCharacter->SetCanInput(true);
		}
		else
		{
			NET_LOG(SourceCharacter, Warning, TEXT("입력 가능 존 끝"))
			SourceCharacter->SetCanInput(false);
			SourceCharacter->SetCanNextAction(false);
		}
	}
}
