// Copyright Surround, Inc. All Rights Reserved.


#include "SMSlashAnimNotify_Inputable.h"

#include "Characters/SMBassCharacter.h"
#include "Characters/Slash/SMSlashComponent.h"
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

	ASMBassCharacter* SourceCharacter = MeshComp->GetOwner<ASMBassCharacter>();
	if (!SourceCharacter)
	{
		return;
	}

	USMSlashComponent* SlashComponent = SourceCharacter->GetSlashComponent();
	if (!ensureAlways(SlashComponent))
	{
		return;
	}


	if (!SourceCharacter->IsLocallyControlled())
	{
		return;
	}

	if (bIsStart)
	{
		InputableZoneEntry(SourceCharacter);
	}
	else
	{
		InputableZoneEscape(SourceCharacter);
	}
}

void USMSlashAnimNotify_Inputable::InputableZoneEntry(ASMBassCharacter* SourceCharacter)
{
	USMSlashComponent* SlashComponent = SourceCharacter->GetSlashComponent();
	if (!ensureAlways(SlashComponent))
	{
		return;
	}

	NET_LOG(SourceCharacter, Warning, TEXT("입력 가능 존 진입"))
	SlashComponent->bCanInput = true;
	SlashComponent->bIsLeftSlashNext = bIsLeftSlashNext;
}

void USMSlashAnimNotify_Inputable::InputableZoneEscape(ASMBassCharacter* SourceCharacter)
{
	USMSlashComponent* SlashComponent = SourceCharacter->GetSlashComponent();
	if (!ensureAlways(SlashComponent))
	{
		return;
	}

	NET_LOG(SourceCharacter, Warning, TEXT("입력 가능 존 탈출"))
	SlashComponent->bCanInput = false;
	SlashComponent->bCanNextAction = false;
}
