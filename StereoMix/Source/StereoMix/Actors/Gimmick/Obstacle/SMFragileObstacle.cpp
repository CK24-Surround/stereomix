﻿// Copyright Studio Surround. All Rights Reserved.


#include "SMFragileObstacle.h"

#include "NiagaraComponent.h"
#include "CADKernel/Math/SlopeUtils.h"
#include "Components/BoxComponent.h"
#include "Net/UnrealNetwork.h"
#include "UObject/ObjectSaveContext.h"
#include "Utilities/SMCollision.h"
#include "Utilities/SMLog.h"


ASMFragileObstacle::ASMFragileObstacle()
{
	DurabilityThresholds.Add({ 0.0f, nullptr });
}

void ASMFragileObstacle::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	CurrentDurability = Durability;
}

void ASMFragileObstacle::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASMFragileObstacle, CurrentDurability);
	DOREPLIFETIME(ASMFragileObstacle, Durability);
	DOREPLIFETIME(ASMFragileObstacle, LastAttacker);
}

void ASMFragileObstacle::PreSave(FObjectPreSaveContext SaveContext)
{
	Super::PreSave(SaveContext);

	DurabilityThresholds.Sort([](const FSMFragileObstacleDurabilityThresholdData& lhs, const FSMFragileObstacleDurabilityThresholdData& rhs) {
		return lhs.DurabilityRatio > rhs.DurabilityRatio;
	});
}

void ASMFragileObstacle::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	SetCollisionEnabled(true);

	if (!bSpawnImmediately)
	{
		SetCollisionEnabled(false);
	}

	DurabilityThresholds.Sort([](const FSMFragileObstacleDurabilityThresholdData& lhs, const FSMFragileObstacleDurabilityThresholdData& rhs) {
		return lhs.DurabilityRatio > rhs.DurabilityRatio;
	});
}

void ASMFragileObstacle::ServerSetCurrentDurability_Implementation(float NewCurrentDurability)
{
	CurrentDurability = NewCurrentDurability;

	OnRep_CurrentDurability();
}

void ASMFragileObstacle::ServerSetMaxDurability_Implementation(float NewMaxDurability)
{
	Durability = NewMaxDurability;

	OnRep_Durability();
}

void ASMFragileObstacle::ServerRestoreObstacle_Implementation()
{
	ServerSetCurrentDurability(Durability);
}

void ASMFragileObstacle::SetCollisionEnabled(bool bNewIsCollisionEnabled)
{
	const FName CollisionProfileName = bNewIsCollisionEnabled ? SMCollisionProfileName::Obstacle : SMCollisionProfileName::NoCollision;
	if (ColliderComponent->GetCollisionProfileName() != CollisionProfileName)
	{
		ColliderComponent->SetCollisionProfileName(CollisionProfileName);
	}
}

void ASMFragileObstacle::UnloadObstacle()
{
	if (CurrentDurability < 0.0f)
	{
		return;
	}

	Super::UnloadObstacle();
}

void ASMFragileObstacle::ReceiveDamage(AActor* NewAttacker, float InDamageAmount)
{
	SetLastAttacker(NewAttacker);

	ServerSetCurrentDurability(FMath::Clamp(CurrentDurability - InDamageAmount, 0.0f, Durability));
	NET_LOG(this, Log, TEXT("%s Damage: %f, CurrentHealth: %f"), *GetName(), InDamageAmount, CurrentDurability);
}

void ASMFragileObstacle::OnRep_CurrentDurability()
{
	UpdateVisualBasedOnDurability();

	SetCollisionEnabled(true);
	if (CurrentDurability <= 0.0f)
	{
		SetCollisionEnabled(false);
	}
}

void ASMFragileObstacle::OnRep_Durability()
{
	UpdateVisualBasedOnDurability();
}

void ASMFragileObstacle::UpdateVisualBasedOnDurability()
{
	UStaticMesh* NewMesh = nullptr;
	UNiagaraSystem* NewNiagaraSystem = nullptr;

	const float CurrentDurabilityRatio = CurrentDurability / Durability;

	for (const FSMFragileObstacleDurabilityThresholdData& DurabilityThreshold : DurabilityThresholds)
	{
		if (FMath::IsNearlyEqual(CurrentDurabilityRatio, DurabilityThreshold.DurabilityRatio, UE::CADKernel::Slope::Epsilon) ||
		    CurrentDurabilityRatio <= DurabilityThreshold.DurabilityRatio)
		{
			NewMesh = DurabilityThreshold.Mesh;
			NewNiagaraSystem = DurabilityThreshold.NiagaraSystem;
		}
	}

	// 내구도가 모든 Threshold보다 높다면 초기 메쉬로 복원
	if (CurrentDurabilityRatio > DurabilityThresholds[0].DurabilityRatio)
	{
		MeshComponent->SetStaticMesh(OriginalMesh);
		NiagaraComponent->SetAsset(OriginalNiagaraSystem);
		return;
	}

	if (NewMesh != MeshComponent->GetStaticMesh())
	{
		MeshComponent->SetStaticMesh(NewMesh);
	}

	if (NewNiagaraSystem != NiagaraComponent->GetAsset())
	{
		NiagaraComponent->SetAsset(NewNiagaraSystem);
	}
}
