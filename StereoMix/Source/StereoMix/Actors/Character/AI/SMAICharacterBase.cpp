// Copyright Studio Surround. All Rights Reserved.


#include "SMAICharacterBase.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
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

	HitBox = CreateDefaultSubobject<UCapsuleComponent>(TEXT("HitBox"));
	HitBox->SetupAttachment(RootComponent);
	HitBox->SetCollisionProfileName(SMCollisionProfileName::Player);
	HitBox->InitCapsuleSize(125.0f, 125.0f);
}

void ASMAICharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
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
}

void ASMAICharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
