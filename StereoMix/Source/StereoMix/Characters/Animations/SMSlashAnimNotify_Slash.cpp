// Copyright Surround, Inc. All Rights Reserved.


#include "SMSlashAnimNotify_Slash.h"

#include "Characters/SMBassCharacter.h"
#include "Characters/Slash/SMSlashComponent.h"
#include "Utilities/SMLog.h"

FString USMSlashAnimNotify_Slash::GetNotifyName_Implementation() const
{
	return TEXT("SMSlashAnimNotifySlash");
}

void USMSlashAnimNotify_Slash::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
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
			NET_LOG(SourceCharacter, Warning, TEXT("휘두르기 시작"));
			SlashComponent->ColliderOrientaionBySlash();
		}
	}
}
