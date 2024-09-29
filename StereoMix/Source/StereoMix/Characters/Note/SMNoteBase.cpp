// Copyright Studio Surround. All Rights Reserved.


#include "SMNoteBase.h"

#include "Animations/SMNoteAnimInstance.h"
#include "Utilities/SMCollision.h"
#include "Utilities/SMLog.h"


ASMNoteBase::ASMNoteBase()
{
	bReplicates = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComponent);

	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetRelativeRotation(FRotator(0.0, 90.0, 0.0));
	MeshComponent->SetCollisionProfileName(SMCollisionProfileName::NoCollision);
	MeshComponent->bReceivesDecals = false;
}

void ASMNoteBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	MeshComponent->SetVisibility(false);
}

void ASMNoteBase::PlayAnimation()
{
	if (USMNoteAnimInstance* AnimInstance = Cast<USMNoteAnimInstance>(MeshComponent->GetAnimInstance()))
	{
		AnimInstance->Montage_Play(AnimInstance->Montage);
	}
}

void ASMNoteBase::StopAnimation()
{
	if (USMNoteAnimInstance* AnimInstance = Cast<USMNoteAnimInstance>(MeshComponent->GetAnimInstance()))
	{
		AnimInstance->Montage_Stop(0.0f);
	}
}
