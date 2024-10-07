// Copyright Studio Surround. All Rights Reserved.


#include "SMObstacleBase.h"

#include "Engine/OverlapResult.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "Actors/Character/Player/SMPlayerCharacterBase.h"
#include "Components/BoxComponent.h"
#include "Utilities/SMCollision.h"
#include "Utilities/SMLog.h"


ASMObstacleBase::ASMObstacleBase()
{
	bReplicates = true;

	ColliderComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("ColliderComponent"));
	RootComponent = ColliderComponent;
	ColliderComponent->SetBoxExtent(FVector(75.0f, 75.0f, 75.0f));
	ColliderComponent->SetCollisionProfileName(SMCollisionProfileName::Obstacle);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionProfileName(SMCollisionProfileName::NoCollision);

	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	NiagaraComponent->SetupAttachment(RootComponent);
	NiagaraComponent->SetCollisionProfileName(SMCollisionProfileName::NoCollision);
}

void ASMObstacleBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	OriginalMesh = MeshComponent->GetStaticMesh();
	OriginalNiagaraSystem = NiagaraComponent->GetAsset();

	MulticastSetCollisionEnabled(false);

	if (bSpawnImmediately)
	{
		MulticastPushBack();
		MulticastSetCollisionEnabled(true);
	}
	else
	{
		MeshComponent->SetStaticMesh(nullptr);
		NiagaraComponent->SetAsset(nullptr);
	}
}

void ASMObstacleBase::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();

	if (!HasAuthority() || !World || bSpawnImmediately)
	{
		return;
	}

	TWeakObjectPtr<ASMObstacleBase> ThisWeakPtr(this);

	float AccumulatedTime = 0.0f;

	auto SetVisual = [ThisWeakPtr](UStaticMesh* NewStaticMesh, UNiagaraSystem* NewNiagaraSystem) {
		if (ThisWeakPtr.Get())
		{
			ThisWeakPtr->MulticastSetMeshAndNiagaraSystem(NewStaticMesh, NewNiagaraSystem);
		}
	};

	auto SetPushBack = [ThisWeakPtr] {
		if (ThisWeakPtr.Get())
		{
			ThisWeakPtr->MulticastPushBack();
		}
	};

	auto SetVisualAndCollision = [ThisWeakPtr](UStaticMesh* NewStaticMesh, UNiagaraSystem* NewNiagaraSystem) {
		if (ThisWeakPtr.Get())
		{
			ThisWeakPtr->MulticastSetCollisionEnabled(true);
			ThisWeakPtr->MulticastSetMeshAndNiagaraSystem(NewStaticMesh, NewNiagaraSystem);
		}
	};

	AccumulatedTime += SpawnDelay;

	FTimerHandle PreSpawnEffectTimerHandle;
	UNiagaraSystem* CachedPreSpawnEffect = PreSpawnEffect;
	AccumulatedTime += PreSpawnEffectDuration;
	World->GetTimerManager().SetTimer(PreSpawnEffectTimerHandle, [SetVisual, CachedPreSpawnEffect] {
		SetVisual(nullptr, CachedPreSpawnEffect);
	}, AccumulatedTime, false);

	FTimerHandle SpawnEffectTimerHandle;
	UNiagaraSystem* CachedSpawnEffect = SpawnEffect;
	AccumulatedTime += SpawnEffectDuration;
	World->GetTimerManager().SetTimer(SpawnEffectTimerHandle, [SetPushBack, SetVisualAndCollision, CachedSpawnEffect] {
		SetPushBack();
		SetVisualAndCollision(nullptr, CachedSpawnEffect);
	}, AccumulatedTime, false);

	FTimerHandle SpawnTimerHandle;
	UStaticMesh* CachedOriginalMesh = OriginalMesh;
	UNiagaraSystem* CachedOriginNiagaraSystem = OriginalNiagaraSystem;
	AccumulatedTime += SpawnEffectDuration;
	World->GetTimerManager().SetTimer(SpawnTimerHandle, [SetVisual, CachedOriginalMesh, CachedOriginNiagaraSystem] {
		SetVisual(CachedOriginalMesh, CachedOriginNiagaraSystem);
	}, AccumulatedTime, false);
}

void ASMObstacleBase::SetCollisionEnabled(bool bNewIsCollisionEnabled)
{
	const FName CollisionProfileName = bNewIsCollisionEnabled ? SMCollisionProfileName::BlockAll : SMCollisionProfileName::NoCollision;
	if (ColliderComponent->GetCollisionProfileName() != CollisionProfileName)
	{
		ColliderComponent->SetCollisionProfileName(CollisionProfileName);
	}
}

void ASMObstacleBase::UnloadObstacle()
{
	MulticastSetCollisionEnabled(false);
	MulticastSetMeshAndNiagaraSystem(nullptr, DestroyEffect);
}

void ASMObstacleBase::MulticastPushBack_Implementation()
{
	UWorld* World = GetWorld();
	if (!HasAuthority() || !World)
	{
		return;
	}

	TArray<FOverlapResult> OverlapResults;
	if (World->OverlapMultiByChannel(OverlapResults, GetActorLocation(), FQuat::Identity, SMCollisionTraceChannel::Action, FCollisionShape::MakeBox(ColliderComponent->GetScaledBoxExtent())))
	{
		for (const FOverlapResult& OverlapResult : OverlapResults)
		{
			AActor* TargetActor = OverlapResult.GetActor();
			if (!TargetActor)
			{
				continue;
			}

			if (ASMPlayerCharacterBase* TargetCharacter = Cast<ASMPlayerCharacterBase>(TargetActor))
			{
				const FVector Direction = (TargetCharacter->GetActorLocation() - GetActorLocation()).GetSafeNormal2D();
				const FRotator Rotation = Direction.Rotation() + FRotator(15.0, 0.0, 0.0);
				const FVector Velocity = Rotation.Vector() * SpawnPushBackMagnitude;
				TargetCharacter->ClientRPCCharacterPushBack(Velocity);
			}
		}
	}
}

void ASMObstacleBase::MulticastSetCollisionEnabled_Implementation(bool bNewIsCollisionEnabled)
{
	SetCollisionEnabled(bNewIsCollisionEnabled);
}

void ASMObstacleBase::MulticastSetMeshAndNiagaraSystem_Implementation(UStaticMesh* NewMesh, UNiagaraSystem* NewNiagaraSystem)
{
	if (!HasAuthority())
	{
		MeshComponent->SetStaticMesh(NewMesh);
		NiagaraComponent->SetAsset(NewNiagaraSystem);
	}
}
