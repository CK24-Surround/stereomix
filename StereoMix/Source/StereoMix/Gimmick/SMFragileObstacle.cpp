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

void ASMFragileObstacle::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASMFragileObstacle, CurrentDurability);
	DOREPLIFETIME(ASMFragileObstacle, MaxDurability);
	DOREPLIFETIME(ASMFragileObstacle, LastAttacker);
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
	CurrentDurability = MaxDurability;

	UpdateMeshBasedOnHealth(CurrentDurability);

	UpdateColliderProfile(SMCollisionProfileName::Obstacle);
}

void ASMFragileObstacle::ReceiveDamage(AActor* NewAttacker, float InDamageAmount)
{
	SetLastAttacker(NewAttacker);
	
	CurrentDurability -= InDamageAmount;
	CurrentDurability = FMath::Clamp(CurrentDurability, 0.0f, MaxDurability);

	NET_LOG(this, Log, TEXT("%s Damage: %f, CurrentHealth: %f"), *GetNameSafe(this), InDamageAmount, CurrentDurability);

	// 체력에 따른 메쉬 변경
	UpdateMeshBasedOnHealth(CurrentDurability);

	if (CurrentDurability <= 0.0f)
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
