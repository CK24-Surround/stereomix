// Copyright Studio Surround. All Rights Reserved.


#include "SMAICharacterBase.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Actors/Notes/SMNoteBase.h"
#include "Actors/Weapons/SMWeaponBase.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/Character/SMHIC_TutorialAI.h"
#include "Components/Common/SMTeamComponent.h"
#include "Components/PlayerController/SMScreenIndicatorComponent.h"
#include "UI/Widget/Game/SMUserWidget_CharacterState.h"
#include "Utilities/SMCollision.h"


ASMAICharacterBase::ASMAICharacterBase()
{
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

	HIC = CreateDefaultSubobject<USMHIC_TutorialAI>(TEXT("HIC"));

	CharacterStateWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("CharacterStateWidgetComponent"));
	CharacterStateWidgetComponent->SetupAttachment(CachedMeshComponent);
	CharacterStateWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	CharacterStateWidgetComponent->SetRelativeLocation(FVector(0.0, 0.0, 300.0));
	CharacterStateWidgetComponent->SetDrawAtDesiredSize(true);

	NoteSlotComponent = CreateDefaultSubobject<USceneComponent>(TEXT("NoteSlotComponent"));
	NoteSlotComponent->SetupAttachment(CachedMeshComponent);
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

	CurrentHP = HP;
}

ESMTeam ASMAICharacterBase::GetTeam() const
{
	return TeamComponent->GetTeam();
}

void ASMAICharacterBase::BeginPlay()
{
	Super::BeginPlay();

	OnChangeHP();
}

void ASMAICharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (USMUserWidget_CharacterState* CharacterStateWidget = CreateWidget<USMUserWidget_CharacterState>(GetWorld(), CharacterStateWidgetClass))
	{
		CharacterStateWidgetComponent->SetWidget(CharacterStateWidget);

		CharacterStateWidget->SetNickname(TEXT("AI"));

		CharacterStateWidget->MaxHealth = HP;
		CharacterStateWidget->CurrentHealth = CurrentHP;
		CharacterStateWidget->UpdateHPBar();
	}
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
		CurrentHP = HP;
		Note->StopAnimation();
		OnChangeHP();
	}
}

void ASMAICharacterBase::ReceiveDamage(AActor* NewAttacker, float InDamageAmount)
{
	UE_LOG(LogTemp, Warning, TEXT("ASMAICharacterBase::ReceiveDamage %f"), InDamageAmount);

	CurrentHP = FMath::Clamp(CurrentHP - InDamageAmount, 0.0f, HP);
	OnChangeHP();
}

void ASMAICharacterBase::AddScreenIndicatorToSelf(AActor* TargetActor)
{
	const ASMPlayerCharacterBase* TargetCharacter = Cast<ASMPlayerCharacterBase>(TargetActor);
	if (!TargetCharacter || TargetCharacter->IsLocallyControlled())
	{
		return;
	}

	// 로컬 컨트롤러를 찾고 스크린 인디케이터를 추가해줍니다.
	const UWorld* World = GetWorld();
	const APlayerController* LocalPlayerController = World ? World->GetFirstPlayerController() : nullptr;
	if (USMScreenIndicatorComponent* ScreenIndicatorComponent = LocalPlayerController ? LocalPlayerController->GetComponentByClass<USMScreenIndicatorComponent>() : nullptr)
	{
		ScreenIndicatorComponent->AddScreenIndicator(TargetActor);
	}
}

void ASMAICharacterBase::RemoveScreenIndicatorFromSelf(AActor* TargetActor)
{
	const ASMPlayerCharacterBase* TargetCharacter = Cast<ASMPlayerCharacterBase>(TargetActor);
	if (!TargetCharacter || TargetCharacter->IsLocallyControlled())
	{
		return;
	}

	// 로컬 컨트롤러를 찾고 스크린 인디케이터를 제거합니다.
	const UWorld* World = GetWorld();
	const APlayerController* LocalPlayerController = World ? World->GetFirstPlayerController() : nullptr;
	if (USMScreenIndicatorComponent* ScreenIndicatorComponent = LocalPlayerController ? LocalPlayerController->GetComponentByClass<USMScreenIndicatorComponent>() : nullptr)
	{
		ScreenIndicatorComponent->RemoveScreenIndicator(TargetActor);
	}
}

void ASMAICharacterBase::OnChangeHP()
{
	if (USMUserWidget_CharacterState* CharacterStateWidget = Cast<USMUserWidget_CharacterState>(CharacterStateWidgetComponent->GetUserWidgetObject()))
	{
		CharacterStateWidget->CurrentHealth = CurrentHP;
		CharacterStateWidget->UpdateHPBar();
	}

	CharacterStateWidgetComponent->SetVisibility(true);
	if (CurrentHP <= 0.0f)
	{
		CharacterStateWidgetComponent->SetVisibility(false);
		AddScreenIndicatorToSelf(this);
		SetNoteState(true);
	}
}

USMHoldInteractionComponent* ASMAICharacterBase::GetHoldInteractionComponent() const
{
	return HIC;
}
