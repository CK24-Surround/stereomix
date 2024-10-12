// Copyright Studio Surround. All Rights Reserved.


#include "SMTrainingDummy.h"

#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/Common/SMTeamComponent.h"
#include "Components/Tutorial/TrainingDummy/SMHIC_TrainingDummy.h"
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

	TeamComponent = CreateDefaultSubobject<USMTeamComponent>(TEXT("TeamComponent"));

	HIC = CreateDefaultSubobject<USMHIC_TrainingDummy>(TEXT("HIC"));
}

void ASMTrainingDummy::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	HP = MaxHP;
}

void ASMTrainingDummy::BeginPlay()
{
	Super::BeginPlay();
}

void ASMTrainingDummy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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

	HP = FMath::Clamp(HP - InDamageAmount, 0.0f, MaxHP);

	if (HP <= 50.0f)
	{
		(void)OnHalfHPReached.ExecuteIfBound();
	}

	if (HP <= 0.0f)
	{
		bIsNeutralized = true;
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

void ASMTrainingDummy::Revival()
{
	HP = MaxHP;
	bIsNeutralized = false;
}
