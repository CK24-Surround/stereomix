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

	DurabilityThresholds.Add({ 0.0f, nullptr });
}

void ASMFragileObstacle::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASMFragileObstacle, CurrentDurability);
	DOREPLIFETIME(ASMFragileObstacle, MaxDurability);
	DOREPLIFETIME(ASMFragileObstacle, LastAttacker);
}

void ASMFragileObstacle::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	OriginalMesh = MeshComponent->GetStaticMesh();

	DurabilityThresholds.Sort([](const FSMFragileObstacleDurabilityThresholdData& lhs, const FSMFragileObstacleDurabilityThresholdData& rhs) {
		return lhs.DurabilityRatio < rhs.DurabilityRatio;
	});
}

void ASMFragileObstacle::ServerSetCurrentDurability_Implementation(float NewCurrentDurability)
{
	CurrentDurability = NewCurrentDurability;

	OnRep_CurrentDurability();
}

void ASMFragileObstacle::ServerSetMaxDurability_Implementation(float NewMaxDurability)
{
	MaxDurability = NewMaxDurability;

	OnRep_MaxDurability();
}

void ASMFragileObstacle::ServerRestoreObstacle_Implementation()
{
	CurrentDurability = MaxDurability;
}

void ASMFragileObstacle::ReceiveDamage(AActor* NewAttacker, float InDamageAmount)
{
	SetLastAttacker(NewAttacker);

	CurrentDurability = FMath::Clamp(CurrentDurability - InDamageAmount, 0.0f, MaxDurability);
	NET_LOG(this, Log, TEXT("%s Damage: %f, CurrentHealth: %f"), *GetName(), InDamageAmount, CurrentDurability);
}

void ASMFragileObstacle::OnRep_CurrentDurability()
{
	UpdateMeshBasedOnDurability();

	SetCollisionEnabled(true);
	if (CurrentDurability <= 0.0f)
	{
		SetCollisionEnabled(false);
	}
}

void ASMFragileObstacle::OnRep_MaxDurability()
{
	UpdateMeshBasedOnDurability();
}

void ASMFragileObstacle::SetCollisionEnabled(bool bNewIsCollisionEnabled)
{
	const FName CollisionProfileName = bNewIsCollisionEnabled ? SMCollisionProfileName::Obstacle : SMCollisionProfileName::NoCollision;
	if (ColliderComponent->GetCollisionProfileName() != CollisionProfileName)
	{
		ColliderComponent->SetCollisionProfileName(CollisionProfileName);
	}
}

void ASMFragileObstacle::UpdateMeshBasedOnDurability()
{
	UStaticMesh* NewMesh = nullptr;
	const float CurrentDurabilityRatio = CurrentDurability / MaxDurability;
	for (const FSMFragileObstacleDurabilityThresholdData& DurabilityThreshold : DurabilityThresholds)
	{
		if (CurrentDurabilityRatio <= DurabilityThreshold.DurabilityRatio)
		{
			NewMesh = DurabilityThreshold.Mesh;
		}
	}

	MeshComponent->SetStaticMesh(NewMesh);

	// 내구도가 모든 Threshold보다 높다면 초기 메쉬로 복원
	if (CurrentDurabilityRatio > DurabilityThresholds[0].DurabilityRatio)
	{
		MeshComponent->SetStaticMesh(OriginalMesh);
	}
}
