// Copyright Studio Surround. All Rights Reserved.


#include "SMObstacleBase.h"

#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
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

	OriginMesh = MeshComponent->GetStaticMesh();
	OriginNiagaraSystem = NiagaraComponent->GetAsset();

	MeshComponent->SetStaticMesh(nullptr);
	NiagaraComponent->SetAsset(nullptr);

	SetCollisionEnabled(false);
}

void ASMObstacleBase::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority())
	{
		return;
	}

	if (SpawnDelay > 0.0f)
	{
		TWeakObjectPtr<ASMObstacleBase> WeakThis = this;

		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, [WeakThis]() {
			WeakThis->ClientSetMeshAndNiagaraSystem(nullptr, WeakThis->DelayEffect);
			FTimerHandle TimerHandle;
			WeakThis->GetWorldTimerManager().SetTimer(TimerHandle, [WeakThis]() {
				WeakThis->ClientSetMeshAndNiagaraSystem(nullptr, WeakThis->SpawnEffect);

				FTimerHandle TimerHandle;
				WeakThis->GetWorldTimerManager().SetTimer(TimerHandle, [WeakThis]() {
					UE_LOG(LogTemp, Warning, TEXT("SpawnEffectDuration: %f"), WeakThis->SpawnEffectDuration);
					WeakThis->MulticastSetCollisionEnabled(true);
					WeakThis->ClientSetMeshAndNiagaraSystem(WeakThis->OriginMesh, WeakThis->OriginNiagaraSystem);
				}, WeakThis->SpawnEffectDuration, false);
			}, WeakThis->SpawnDelay, false);
		}, 3.0f, false);

		return;
	}

	// 바로 콜리전 활성화 및 원래 메시와 이펙트 설정
	MulticastSetCollisionEnabled(true);
	ClientSetMeshAndNiagaraSystem(OriginMesh, OriginNiagaraSystem);
}

void ASMObstacleBase::SetCollisionEnabled(bool bNewIsCollisionEnabled)
{
	const FName CollisionProfileName = bNewIsCollisionEnabled ? SMCollisionProfileName::Obstacle : SMCollisionProfileName::NoCollision;
	if (ColliderComponent->GetCollisionProfileName() != CollisionProfileName)
	{
		ColliderComponent->SetCollisionProfileName(CollisionProfileName);
	}
}

void ASMObstacleBase::MulticastSetCollisionEnabled_Implementation(bool bNewIsCollisionEnabled)
{
	SetCollisionEnabled(bNewIsCollisionEnabled);
}

void ASMObstacleBase::ClientSetMeshAndNiagaraSystem_Implementation(UStaticMesh* NewMesh, UNiagaraSystem* NewNiagaraSystem)
{
	UE_LOG(LogTemp, Warning, TEXT("ClientSetMeshAndNiagaraSystem_Implementation"));
	if (!HasAuthority())
	{
		MeshComponent->SetStaticMesh(NewMesh);
		NiagaraComponent->SetAsset(NewNiagaraSystem);
	}
}
