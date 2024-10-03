// Copyright Studio Surround. All Rights Reserved.


#include "SMNoteBase.h"

#include "NiagaraComponent.h"
#include "Animations/Note/SMNoteAnimInstance.h"
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

	GlowFXComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("GlowFXComponent"));
	GlowFXComponent->SetupAttachment(MeshComponent);

	ParticleFXComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ParticleFXComponent"));
	ParticleFXComponent->SetupAttachment(RootComponent);

	FloorFXComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("FloorFXComponent"));
	FloorFXComponent->SetupAttachment(RootComponent);
}

void ASMNoteBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	const FName SocketName = TEXT("PRoot");
	GlowFXComponent->AttachToComponent(MeshComponent, FAttachmentTransformRules::KeepRelativeTransform, SocketName);

	const FVector ZOffset = MeshComponent->GetSocketTransform(SocketName, RTS_Actor).GetLocation();
	ParticleFXComponent->SetRelativeLocation(ZOffset);
}

void ASMNoteBase::BeginPlay()
{
	Super::BeginPlay();

	MeshComponent->SetVisibility(false);
	GlowFXComponent->SetVisibility(false);
	ParticleFXComponent->SetVisibility(false);
	FloorFXComponent->SetVisibility(false);
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
