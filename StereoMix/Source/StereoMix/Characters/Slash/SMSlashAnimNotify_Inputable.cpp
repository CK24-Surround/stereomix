// Copyright Surround, Inc. All Rights Reserved.


#include "SMSlashAnimNotify_Inputable.h"

#include "Characters/Player/SMBassCharacter.h"
#include "Characters/Slash/SMSlashComponent.h"
#include "Utilities/SMLog.h"

FString USMSlashAnimNotify_Inputable::GetNotifyName_Implementation() const
{
	return TEXT("SlashNotify_Inputable");
}

void USMSlashAnimNotify_Inputable::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
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

	SlashComponent->bCanInput = true;
}
