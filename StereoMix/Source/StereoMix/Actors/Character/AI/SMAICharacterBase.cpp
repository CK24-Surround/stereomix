// Copyright Studio Surround. All Rights Reserved.


#include "SMAICharacterBase.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraComponentPoolMethodEnum.h"
#include "NiagaraFunctionLibrary.h"
#include "Actors/Character/Player/SMPlayerCharacterBase.h"
#include "Actors/Notes/SMNoteBase.h"
#include "Actors/Weapons/SMWeaponBase.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/Character/SMHIC_Character.h"
#include "Components/Character/SMHIC_TutorialAI.h"
#include "Components/Common/SMTeamComponent.h"
#include "Components/PlayerController/SMScreenIndicatorComponent.h"
#include "FunctionLibraries/SMHoldInteractionBlueprintLibrary.h"
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

	const USkeletalMeshComponent* CachedMeshComponent = GetMesh();

	OriginalMaterials = CachedMeshComponent->GetMaterials();
	OriginalOverlayMaterial = CachedMeshComponent->GetOverlayMaterial();

	if (UWorld* World = GetWorld())
	{
		if (Weapon = World->SpawnActor<ASMWeaponBase>(WeaponClass); Weapon)
		{
			Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocketName);
			Weapon->SetOwner(this);
		}

		if (Note = World->SpawnActor<ASMNoteBase>(NoteClass); Note)
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

bool ASMAICharacterBase::CanIgnoreAttack() const
{
	if (bCanAttack)
	{
		return true;
	}

	return false;
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

	CharacterMeshComponent->SetVisibility(!bIsNoteState);

	WeaponRootComponent->SetVisibility(!bIsNoteState, true);
	NoteRootComponent->SetVisibility(bIsNoteState, true);

	if (bIsNoteState)
	{
		Note->PlayAnimation();
		RegisterNoteStateEndTimer(3.0f);
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

void ASMAICharacterBase::DestroyNoteStateEndTimer()
{
	if (const UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(NoteStateEndTimerHandle);
	}
}

void ASMAICharacterBase::RegisterNoteStateEndTimer(float Duration)
{
	DestroyNoteStateEndTimer();

	if (const UWorld* World = GetWorld())
	{
		TWeakObjectPtr<ASMAICharacterBase> ThisWeakPtr = TWeakObjectPtr<ASMAICharacterBase>(this);
		World->GetTimerManager().SetTimer(NoteStateEndTimerHandle, [ThisWeakPtr] {
			USMHoldInteractionComponent* HIC = ThisWeakPtr->GetHoldInteractionComponent();
			ASMPlayerCharacterBase* TargetCharacter = Cast<ASMPlayerCharacterBase>(HIC->GetActorHoldingMe());
			USMHIC_Character* TargetHIC = Cast<USMHIC_Character>(USMHoldInteractionBlueprintLibrary::GetHoldInteractionComponent(TargetCharacter));

			if (USMHIC_TutorialAI* TutorialAI = Cast<USMHIC_TutorialAI>(HIC))
			{
				TutorialAI->ReleasedFromBeingHeld(TargetCharacter);
			}

			if (TargetHIC)
			{
				TargetHIC->SetActorIAmHolding(nullptr);
			}
		}, Duration, false);
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
		bCanAttack = false;
		CharacterStateWidgetComponent->SetVisibility(false);
		AddScreenIndicatorToSelf(this);
		SetNoteState(true);
	}
}

void ASMAICharacterBase::SetGhostMaterial(float Duration)
{
	USkeletalMeshComponent* SourceMesh = GetMesh();
	const ASMWeaponBase* SourceWeapon = GetWeapon();
	UMeshComponent* SourceWeaponMesh = SourceWeapon ? SourceWeapon->GetWeaponMeshComponent() : nullptr;
	if (!SourceMesh || !SourceWeaponMesh)
	{
		return;
	}

	VFXComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(ImmuneStartVFX, GetRootComponent(), NAME_None, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, false, true, ENCPoolMethod::ManualRelease);

	for (int32 i = 0; i < SourceMesh->GetNumMaterials(); ++i)
	{
		SourceMesh->SetMaterial(i, ImmuneMaterial);
	}

	for (int32 i = 0; i < SourceWeaponMesh->GetNumMaterials(); ++i)
	{
		SourceWeaponMesh->SetMaterial(i, ImmuneMaterial);
	}

	SourceMesh->SetOverlayMaterial(ImmuneOverlayMaterial);
	SourceWeaponMesh->SetOverlayMaterial(ImmuneOverlayMaterial);

	if (const UWorld* World = GetWorld())
	{
		FTimerHandle TimerHandle;
		TWeakObjectPtr<ASMAICharacterBase> ThisWeakPtr = TWeakObjectPtr<ASMAICharacterBase>(this);
		World->GetTimerManager().SetTimer(TimerHandle, [ThisWeakPtr, SourceMesh, SourceWeaponMesh] {
			if (ThisWeakPtr.IsValid())
			{
				ThisWeakPtr->bCanAttack = true;

				if (ThisWeakPtr->VFXComponent)
				{
					ThisWeakPtr->VFXComponent->Deactivate();
					ThisWeakPtr->VFXComponent->ReleaseToPool();
					ThisWeakPtr->VFXComponent = nullptr;
				}

				for (int32 i = 0; i < SourceMesh->GetNumMaterials(); ++i)
				{
					SourceMesh->SetMaterial(i, ThisWeakPtr->OriginalMaterials[i]);
				}

				for (int32 i = 0; i < SourceWeaponMesh->GetNumMaterials(); ++i)
				{
					SourceWeaponMesh->SetMaterial(i, ThisWeakPtr->OriginalMaterials[i]);
				}

				SourceMesh->SetOverlayMaterial(ThisWeakPtr->OriginalOverlayMaterial);
				SourceWeaponMesh->SetOverlayMaterial(ThisWeakPtr->OriginalOverlayMaterial);

				UNiagaraFunctionLibrary::SpawnSystemAttached(ThisWeakPtr->ImmuneEndVFX, ThisWeakPtr->GetRootComponent(), NAME_None, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, false, true, ENCPoolMethod::AutoRelease);
			}
		}, Duration, false);
	}
}

USMHoldInteractionComponent* ASMAICharacterBase::GetHoldInteractionComponent() const
{
	return HIC;
}
