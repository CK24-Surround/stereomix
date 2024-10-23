// Copyright Studio Surround. All Rights Reserved.


#include "SMTrainingDummy.h"

#include "NiagaraFunctionLibrary.h"
#include "Actors/Notes/SMNoteBase.h"
#include "Actors/Weapons/SMWeaponBase.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/Common/SMTeamComponent.h"
#include "Components/Tutorial/TrainingDummy/SMHIC_TrainingDummy.h"
#include "Data/DataAsset/Character/SMElectricGuitarCharacterDataAsset.h"
#include "UI/Widget/Game/SMUserWidget_TrainingDummyState.h"
#include "Utilities/SMCollision.h"


ASMTrainingDummy::ASMTrainingDummy()
{
	PrimaryActorTick.bCanEverTick = false;

	UCapsuleComponent* CachedCapsule = GetCapsuleComponent();
	CachedCapsule->SetCollisionProfileName(SMCollisionProfileName::Player);

	ColliderComponent = CreateDefaultSubobject<USphereComponent>(TEXT("ColliderComponent"));
	ColliderComponent->SetupAttachment(RootComponent);
	ColliderComponent->SetCollisionProfileName(SMCollisionProfileName::PlayerProjectileHitbox);
	ColliderComponent->InitSphereRadius(150.0f);

	USkeletalMeshComponent* CachedMesh = GetMesh();
	CachedMesh->SetupAttachment(RootComponent);
	CachedMesh->SetCollisionProfileName(SMCollisionProfileName::NoCollision);
	CachedMesh->bRenderCustomDepth = true;
	CachedMesh->SetCustomDepthStencilValue(static_cast<int32>(ESMShaderStencil::EnemyOutline));
	CachedMesh->bReceivesDecals = false;

	NoteRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("NoteRootComponent"));
	NoteRootComponent->SetAbsolute(false, true, true);
	NoteRootComponent->SetupAttachment(CachedMesh);

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

	if (Weapon = World ? World->SpawnActor<ASMWeaponBase>(WeaponClass) : nullptr; Weapon)
	{
		const USMElectricGuitarCharacterDataAsset* ElectricGuitarDataAssetCDO = USMElectricGuitarCharacterDataAsset::StaticClass()->GetDefaultObject<USMElectricGuitarCharacterDataAsset>();
		const FName SocketName = ElectricGuitarDataAssetCDO ? ElectricGuitarDataAssetCDO->WeaponSocketName : NAME_None;
		Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, SocketName);

		if (UMeshComponent* WeaponMesh = Weapon->GetWeaponMeshComponent())
		{
			WeaponMesh->SetCustomDepthStencilValue(static_cast<int32>(ESMShaderStencil::EnemyOutline));
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

	if (SpawnVFX)
	{
		UNiagaraFunctionLibrary::SpawnSystemAttached(SpawnVFX, GetRootComponent(), NAME_None, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, false, true, ENCPoolMethod::AutoRelease);
	}
}

void ASMTrainingDummy::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (Note)
	{
		Note->Destroy();
	}

	if (Weapon)
	{
		Weapon->Destroy();
	}

	Super::EndPlay(EndPlayReason);
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

	if (Weapon)
	{
		Weapon->SetActorHiddenInGame(bNewHidden);
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

	if (USkeletalMeshComponent* CachedMesh = GetMesh())
	{
		CachedMesh->SetVisibility(!bNewIsNoteState);
	}

	if (USceneComponent* WeaponRoot = Weapon ? Weapon->GetRootComponent() : nullptr)
	{
		WeaponRoot->SetVisibility(!bNewIsNoteState, true);
	}

	bNewIsNoteState ? Note->PlayAnimation() : Note->StopAnimation();
}
