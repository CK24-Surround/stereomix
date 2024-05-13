// Copyright Surround, Inc. All Rights Reserved.


#include "SMItemSpawner.h"

#include "SMItem.h"
#include "Games/SMGameMode.h"
#include "Utilities/SMCollision.h"
#include "Utilities/SMLog.h"


ASMItemSpawner::ASMItemSpawner()
{
	bReplicates = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(SceneComponent);
	MeshComponent->SetCollisionProfileName(SMCollisionProfileName::NoCollision);

	ItemSocketComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ItemSocketComponent"));
	ItemSocketComponent->SetupAttachment(MeshComponent);
}

void ASMItemSpawner::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		ASMGameMode* SMGameMode = GetWorld()->GetAuthGameMode<ASMGameMode>();
		if (ensureAlways(SMGameMode))
		{
			SMGameMode->OnStartMatch.AddUObject(this, &ThisClass::SpawnTimerStart);
		}
	}
}

void ASMItemSpawner::SpawnTimerStart()
{
	NET_LOG(this, Warning, TEXT(""));
	ResetDelegate();

	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ThisClass::SpawnTimerCallback, SpawnCooldown, false);
}

void ASMItemSpawner::SpawnTimerCallback()
{
	SpawnItem();
}

void ASMItemSpawner::SpawnItem()
{
	NET_LOG(this, Warning, TEXT(""));
	Item = GetWorld()->SpawnActor<ASMItem>(SpawnItemClass, ItemSocketComponent->GetComponentTransform());
	if (ensureAlways(Item))
	{
		// 아이템이 사용되거나 파괴되면 다시 타이머를 시작합니다.
		Item->OnUsedItem.BindUObject(this, &ThisClass::OnUsedItem);
		Item->OnDestroyed.AddDynamic(this, &ThisClass::OnDestroyedItems);
	}
}

void ASMItemSpawner::OnUsedItem()
{
	NET_LOG(this, Warning, TEXT(""));
	SpawnTimerStart();
}

void ASMItemSpawner::OnDestroyedItems(AActor* DestroyedActor)
{
	NET_LOG(this, Warning, TEXT(""));
	SpawnTimerStart();
}

void ASMItemSpawner::ResetDelegate()
{
	if (Item)
	{
		NET_LOG(this, Warning, TEXT(""));
		Item->OnUsedItem.Unbind();
		Item->OnDestroyed.RemoveAll(this);
	}
}
