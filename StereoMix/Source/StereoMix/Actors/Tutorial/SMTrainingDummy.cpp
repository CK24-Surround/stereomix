// Copyright Studio Surround. All Rights Reserved.


#include "SMTrainingDummy.h"

#include "Actors/Notes/SMNoteBase.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/Common/SMTeamComponent.h"
#include "Components/Tutorial/TrainingDummy/SMHIC_TrainingDummy.h"
#include "UI/Widget/Game/SMUserWidget_TrainingDummyState.h"
#include "Utilities/SMCollision.h"


ASMTrainingDummy::ASMTrainingDummy()
{
	PrimaryActorTick.bCanEverTick = false;

	RootCapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("RootCapsuleComponent"));
	RootComponent = RootCapsuleComponent;
	RootCapsuleComponent->SetCollisionProfileName(SMCollisionProfileName::Player);

	ColliderComponent = CreateDefaultSubobject<USphereComponent>(TEXT("ColliderComponent"));
	ColliderComponent->SetupAttachment(RootComponent);
	ColliderComponent->SetCollisionProfileName(SMCollisionProfileName::PlayerProjectileHitbox);
	ColliderComponent->InitSphereRadius(150.0f);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionProfileName(SMCollisionProfileName::NoCollision);
	MeshComponent->bReceivesDecals = false;

	NoteRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("NoteRootComponent"));
	NoteRootComponent->SetAbsolute(false, true, true);
	NoteRootComponent->SetupAttachment(MeshComponent);

	StateWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPBarWidgetComponent"));
	StateWidgetComponent->SetupAttachment(RootComponent);
	StateWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);

	TeamComponent = CreateDefaultSubobject<USMTeamComponent>(TEXT("TeamComponent"));

	HIC = CreateDefaultSubobject<USMHIC_TrainingDummy>(TEXT("HIC"));
}

void ASMTrainingDummy::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	UWorld* World = GetWorld();
	if (Note = World ? World->SpawnActor<ASMNoteBase>(NoteClass) : nullptr; Note)
	{
		Note->AttachToComponent(NoteRootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		if (USceneComponent* NoteComponent = Note->GetRootComponent())
		{
			NoteComponent->SetVisibility(false, true);
		}
	}

	HP = MaxHP;
}

void ASMTrainingDummy::BeginPlay()
{
	Super::BeginPlay();

	if (USMUserWidget_TrainingDummyState* TrainingDummyStateWidget = CreateWidget<USMUserWidget_TrainingDummyState>(GetWorld(), TrainingDummyStateWidgetClass))
	{
		StateWidgetComponent->SetWidget(TrainingDummyStateWidget);
		OnHPChanged.BindUObject(TrainingDummyStateWidget, &USMUserWidget_TrainingDummyState::SetHP);
		TrainingDummyStateWidget->SetHP(HP, MaxHP);
	}
}

void ASMTrainingDummy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASMTrainingDummy::SetActorHiddenInGame(bool bNewHidden)
{
	Super::SetActorHiddenInGame(bNewHidden);

	if (Note)
	{
		Note->SetActorHiddenInGame(bNewHidden);
	}
}

ESMTeam ASMTrainingDummy::GetTeam() const
{
	return TeamComponent->GetTeam();
}

void ASMTrainingDummy::ReceiveDamage(AActor* NewAttacker, float InDamageAmount)
{
	if (bIsInvincible)
	{
		return;
	}

	SetCurrentHP(FMath::Clamp(HP - InDamageAmount, 0.0f, MaxHP));

	if (HP <= 50.0f)
	{
		(void)OnHalfHPReached.ExecuteIfBound();
	}

	if (HP <= 0.0f)
	{
		SetNoteState(true);

		(void)OnNeutralized.ExecuteIfBound();
	}
}

bool ASMTrainingDummy::CanIgnoreAttack() const
{
	if (bIsNeutralized)
	{
		return true;
	}

	return false;
}

USMHoldInteractionComponent* ASMTrainingDummy::GetHoldInteractionComponent() const
{
	return HIC;
}

void ASMTrainingDummy::SetCurrentHP(float InCurrentHP)
{
	HP = InCurrentHP;

	(void)OnHPChanged.ExecuteIfBound(HP, MaxHP);
}

void ASMTrainingDummy::Revival()
{
	SetNoteState(false);
	SetCurrentHP(MaxHP);
}

void ASMTrainingDummy::SetNoteState(bool bNewIsNoteState)
{
	if (USceneComponent* NoteComponent = Note ? Note->GetRootComponent() : nullptr)
	{
		NoteComponent->SetVisibility(bNewIsNoteState, true);
	}

	bIsNeutralized = bNewIsNoteState;

	StateWidgetComponent->SetVisibility(!bNewIsNoteState);
	MeshComponent->SetVisibility(!bNewIsNoteState);

	bNewIsNoteState ? Note->PlayAnimation() : Note->StopAnimation();
}
