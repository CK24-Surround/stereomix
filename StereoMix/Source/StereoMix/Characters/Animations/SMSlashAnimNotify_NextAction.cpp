// Copyright Surround, Inc. All Rights Reserved.


#include "SMSlashAnimNotify_NextAction.h"

#include "Characters/SMBassCharacter.h"
#include "Characters/Slash/SMSlashComponent.h"
#include "Utilities/SMLog.h"

FString USMSlashAnimNotify_NextAction::GetNotifyName_Implementation() const
{
	return TEXT("SlashNextActionNotify");
}

void USMSlashAnimNotify_NextAction::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp)
	{
		return;
	}

	ASMBassCharacter* SourceCharacter = MeshComp->GetOwner<ASMBassCharacter>();
	if (SourceCharacter)
	{
		if (!SourceCharacter->IsLocallyControlled())
		{
			return;
		}

		USMSlashComponent* SlashComponent = SourceCharacter->GetSlashComponent();
		if (ensureAlways(SlashComponent))
		{
			NET_LOG(SourceCharacter, Warning, TEXT("다음 콤보로 넘어가는 포인트"));
			SlashComponent->bCanNextAction = true;
		}
	}
}
