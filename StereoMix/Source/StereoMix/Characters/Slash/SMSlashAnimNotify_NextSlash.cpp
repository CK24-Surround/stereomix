// Copyright Surround, Inc. All Rights Reserved.


#include "SMSlashAnimNotify_NextSlash.h"

#include "SMSlashComponent.h"
#include "Characters/Player/SMBassCharacter.h"

FString USMSlashAnimNotify_NextSlash::GetNotifyName_Implementation() const
{
	return TEXT("SlashNotify_NextSlash");
}

void USMSlashAnimNotify_NextSlash::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp)
	{
		return;
	}

	ASMBassCharacter* SourceCharacter = MeshComp->GetOwner<ASMBassCharacter>();
	if (!SourceCharacter)
	{
		return;
	}

	if (!SourceCharacter->IsLocallyControlled())
	{
		return;
	}

	USMSlashComponent* SlashComponent = SourceCharacter->GetSlashComponent();
	if (!ensureAlways(SlashComponent))
	{
		return;
	}

	SlashComponent->bIsLeftSlashNext = bIsLeftSlashNext;
}
