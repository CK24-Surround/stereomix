// Copyright Studio Surround. All Rights Reserved.


#include "SMFragileObstacle.h"

#include "Components/BoxComponent.h"
#include "Net/UnrealNetwork.h"
#include "Utilities/SMCollision.h"
#include "Utilities/SMLog.h"


ASMFragileObstacle::ASMFragileObstacle()
{
	bReplicates = true;
	
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;
	
	ColliderComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("ColliderComponent"));
	ColliderComponent->SetupAttachment(RootComponent);
	ColliderComponent->SetBoxExtent(FVector(75.0f, 75.0f, 75.0f));
	ColliderComponent->SetCollisionProfileName(SMCollisionProfileName::Obstacle);
	
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionProfileName(SMCollisionProfileName::NoCollision);
}

void ASMFragileObstacle::BeginPlay()
{
	Super::BeginPlay();

	if (MeshComponent && MeshComponent->GetStaticMesh())
	{
		InitialMesh = MeshComponent->GetStaticMesh();
	}
}

void ASMFragileObstacle::InitObstacle()
{
	CurrentHealth = MaxHealth;

	UpdateMeshBasedOnHealth(CurrentHealth);

	UpdateColliderProfile(SMCollisionProfileName::Obstacle);
}

void ASMFragileObstacle::HandleDamage(float InDamage)
{
	CurrentHealth -= InDamage;
	CurrentHealth = FMath::Clamp(CurrentHealth, 0.0f, MaxHealth);

	// 체력에 따른 메쉬 변경
	UpdateMeshBasedOnHealth(CurrentHealth);
	
	if (CurrentHealth <= 0.0f)
	{
		UpdateColliderProfile(SMCollisionProfileName::NoCollision);
	}
}

void ASMFragileObstacle::UpdateColliderProfile_Implementation(FName InProfileName)
{
	ColliderComponent->SetCollisionProfileName(InProfileName);
}

void ASMFragileObstacle::UpdateMeshBasedOnHealth_Implementation(float InHealth)
{
	UStaticMesh* NewMesh = nullptr;
	
	for (int32 i = 0; i < HealthThresholds.Num(); i++)
	{
		if (InHealth <= HealthThresholds[i] && HealthThresholdMeshes.IsValidIndex(i))
		{
			NewMesh = HealthThresholdMeshes[i];
		}
	}

	MeshComponent->SetStaticMesh(NewMesh);
	
	// 체력이 모든 Threshold보다 높다면 초기 메쉬로 복원
	if (InHealth > HealthThresholds[0])
	{
		MeshComponent->SetStaticMesh(InitialMesh);
	}
}

void ASMFragileObstacle::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASMFragileObstacle, CurrentHealth);
	DOREPLIFETIME(ASMFragileObstacle, MaxHealth);
}
