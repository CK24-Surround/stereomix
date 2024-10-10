// Copyright Studio Surround. All Rights Reserved.


#include "SMTrainingDummy.h"

#include "Components/CapsuleComponent.h"
#include "Components/Common/SMTeamComponent.h"
#include "Components/Tutorial/TrainingDummy/SMHIC_TrainingDummy.h"
#include "Utilities/SMCollision.h"


ASMTrainingDummy::ASMTrainingDummy()
{
	PrimaryActorTick.bCanEverTick = false;

	RootCapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("RootCapsuleComponent"));
	RootComponent = RootCapsuleComponent;
	RootCapsuleComponent->SetCollisionProfileName(SMCollisionProfileName::Player);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionProfileName(SMCollisionProfileName::NoCollision);
	
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
	HP = FMath::Clamp(HP - InDamageAmount, 0.0f, MaxHP);

	if (HP <= 50.0f)
	{
		(void)OnHalfHPReached.ExecuteIfBound();
	}

	if (HP <= 0.0f)
	{
		(void)OnNeutralized.ExecuteIfBound();
	}
}

USMHoldInteractionComponent* ASMTrainingDummy::GetHoldInteractionComponent() const
{
	return HIC;
}
