// Copyright Surround, Inc. All Rights Reserved.


#include "SMPreviewCharacter.h"

#include "FMODBlueprintStatics.h"
#include "Components/SceneCaptureComponent2D.h"


ASMPreviewCharacter::ASMPreviewCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));

	CharacterMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMeshComponent"));
	CharacterMeshComponent->SetupAttachment(SceneComponent);
	// CharacterMeshComponent->SetSkeletalMesh(CharacterMesh);
	// CharacterMeshComponent->SetAnimInstanceClass(CharacterAnimInstanceClass);
	
	SceneCaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCaptureComponent"));
	SceneCaptureComponent->SetupAttachment(SceneComponent);
	SceneCaptureComponent->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_RenderScenePrimitives;
	SceneCaptureComponent->CaptureSource = SCS_FinalToneCurveHDR;
	SceneCaptureComponent->ShowOnlyComponent(CharacterMeshComponent);
}

void ASMPreviewCharacter::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
	CharacterMeshComponent->SetSkeletalMesh(CharacterMesh);
	CharacterMeshComponent->SetAnimInstanceClass(CharacterAnimInstanceClass);
	SceneCaptureComponent->TextureTarget = CharacterRenderTarget;
}

void ASMPreviewCharacter::PlaySelectAnimation() const
{
	if (CharacterSelectAnimMontage)
	{
		CharacterMeshComponent->GetAnimInstance()->Montage_Play(CharacterSelectAnimMontage);
	}
	if (CharacterSelectSound)
	{
		UFMODBlueprintStatics::PlayEvent2D(GetWorld(), CharacterSelectSound, true);
	}
}

void ASMPreviewCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ASMPreviewCharacter::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASMPreviewCharacter::InitPlayerState(ASMPlayerState* PlayerState)
{
	OwningPlayerState = PlayerState;
}

void ASMPreviewCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

