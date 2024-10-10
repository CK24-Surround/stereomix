// Copyright Studio Surround. All Rights Reserved.


#include "SMAICharacterBase.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Actors/Notes/SMNoteBase.h"
#include "Actors/Weapons/SMWeaponBase.h"
#include "Components/CapsuleComponent.h"
#include "Components/Common/SMTeamComponent.h"
#include "Utilities/SMCollision.h"


ASMAICharacterBase::ASMAICharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	USkeletalMeshComponent* CachedMeshComponent = GetMesh();
	CachedMeshComponent->SetCollisionProfileName(SMCollisionProfileName::NoCollision);
	CachedMeshComponent->SetRelativeRotation(FRotator(0.0, -90.0, 0.0));
	CachedMeshComponent->bRenderCustomDepth = true;
	CachedMeshComponent->bReceivesDecals = false;

	UCharacterMovementComponent* CachedMovementComponent = GetCharacterMovement();
	CachedMovementComponent->MaxAcceleration = 9999.0f;
	CachedMovementComponent->MaxStepHeight = 10.0f;
	CachedMovementComponent->SetWalkableFloorAngle(5.0f);
	CachedMovementComponent->bCanWalkOffLedges = false;

	NoteSlotComponent = CreateDefaultSubobject<USceneComponent>(TEXT("NoteSlotComponent"));
	NoteSlotComponent->SetupAttachment(CachedMeshComponent);
	NoteSlotComponent->SetAbsolute(false, true, false);

	NoteSlotComponent->SetAbsolute(false, true, false);
	HitBox = CreateDefaultSubobject<UCapsuleComponent>(TEXT("HitBox"));
	HitBox->SetupAttachment(RootComponent);
	HitBox->SetCollisionProfileName(SMCollisionProfileName::Player);
	HitBox->InitCapsuleSize(125.0f, 125.0f);

	TeamComponent = CreateDefaultSubobject<USMTeamComponent>(TEXT("Team"));
}

void ASMAICharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (UWorld* World = GetWorld())
	{
		Weapon = World->SpawnActor<ASMWeaponBase>(WeaponClass);
		if (Weapon)
		{
			Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocketName);
			Weapon->SetOwner(this);
		}

		Note = World->SpawnActor<ASMNoteBase>(NoteClass);
		if (Note)
		{
			Note->AttachToComponent(NoteSlotComponent, FAttachmentTransformRules::KeepRelativeTransform);
			Note->SetOwner(this);
		}
	}
}

ESMTeam ASMAICharacterBase::GetTeam() const
{
	return TeamComponent->GetTeam();
}

void ASMAICharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

void ASMAICharacterBase::SetNoteState(bool bNewIsNote)
{
	if (bIsNoteState == bNewIsNote)
	{
		return;
	}

	bIsNoteState = bNewIsNote;

	USkeletalMeshComponent* CharacterMeshComponent = GetMesh();
	USceneComponent* WeaponRootComponent = Weapon ? Weapon->GetRootComponent() : nullptr;
	USceneComponent* NoteRootComponent = Note ? Note->GetRootComponent() : nullptr;
	if (!CharacterMeshComponent || !WeaponRootComponent || !NoteRootComponent)
	{
		return;
	}

	HitBox->SetCollisionProfileName(bIsNoteState ? SMCollisionProfileName::HoldableItem : SMCollisionProfileName::Player);
	
	CharacterMeshComponent->SetVisibility(!bIsNoteState);

	WeaponRootComponent->SetVisibility(!bIsNoteState, true);
	NoteRootComponent->SetVisibility(bIsNoteState, true);

	if (bIsNoteState)
	{
		Note->PlayAnimation();
	}
	else
	{
		Note->StopAnimation();
	}
}

AActor* ASMAICharacterBase::GetLastAttacker() const
{
	return nullptr;
}

void ASMAICharacterBase::SetLastAttacker(AActor* NewAttacker)
{
}

void ASMAICharacterBase::ReceiveDamage(AActor* NewAttacker, float InDamageAmount)
{
	UE_LOG(LogTemp, Warning, TEXT("ASMAICharacterBase::ReceiveDamage"));

	SetNoteState(true);
}

void ASMAICharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
