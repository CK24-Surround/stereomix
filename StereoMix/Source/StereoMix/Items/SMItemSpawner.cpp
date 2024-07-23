// Copyright Surround, Inc. All Rights Reserved.


#include "SMItemSpawner.h"

#include "Games/SMGameMode.h"
#include "SMItem.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "UI/Widget/Item/SMItemSpawnTimerWidget.h"
#include "Utilities/SMCollision.h"
#include "Utilities/SMLog.h"


ASMItemSpawner::ASMItemSpawner()
{
	bReplicates = true;
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(SceneComponent);
	MeshComponent->SetCollisionProfileName(SMCollisionProfileName::NoCollision);

	ItemSocketComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ItemSocketComponent"));
	ItemSocketComponent->SetupAttachment(MeshComponent);

	ItemSpawnTimerWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("ItemSpawnTimerWidgetComponent"));
	ItemSpawnTimerWidgetComponent->SetupAttachment(SceneComponent);
}
 
void ASMItemSpawner::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		ASMGameMode* SMGameMode = GetWorld()->GetAuthGameMode<ASMGameMode>();
		if (ensureAlways(SMGameMode))
		{
			if (bUseImmediatelySpawn)
			{
				SMGameMode->OnStartMatch.AddDynamic(this, &ThisClass::SpawnItem);
			}
			else
			{
				SMGameMode->OnStartMatch.AddDynamic(this, &ThisClass::SpawnTimerStart);
			}
		}
	}

	USMItemSpawnTimerWidget* ItemSpawnTimerWidget = Cast<USMItemSpawnTimerWidget>(ItemSpawnTimerWidgetComponent->GetWidget());
	if (ItemSpawnTimerWidget)
	{
		ItemSpawnTimerWidget->SetItemSpawner(this);
		float RemainingCooldownPercent = FMath::Clamp(RemainingCooldownSeconds <= 0.0f ? 1.0f : 1.0f - RemainingCooldownSeconds / SpawnCooldown, 0.0f, 1.0f);
		OnRemainingCooldownChanged.Broadcast(RemainingCooldownSeconds, RemainingCooldownPercent);
	}
}

void ASMItemSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateCooldown(DeltaTime);
}

void ASMItemSpawner::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASMItemSpawner, RemainingCooldownSeconds);
}

void ASMItemSpawner::SpawnTimerStart()
{
	ResetSpawnerState();

	RemainingCooldownSeconds = SpawnCooldown;

	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ThisClass::SpawnTimerCallback, SpawnCooldown, false);
}

void ASMItemSpawner::SpawnTimerCallback()
{
	SpawnItem();
}

void ASMItemSpawner::SpawnItem()
{
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
	SpawnTimerStart();
}

void ASMItemSpawner::OnDestroyedItems(AActor* DestroyedActor)
{
	SpawnTimerStart();
}

void ASMItemSpawner::ResetSpawnerState()
{
	if (Item)
	{
		Item->OnUsedItem.Unbind();
		Item->OnDestroyed.RemoveAll(this);
	}

	Item = nullptr;
}

void ASMItemSpawner::UpdateCooldown(float DeltaTime)
{
	if (RemainingCooldownSeconds > 0.0f)
	{
		RemainingCooldownSeconds = FMath::Max(RemainingCooldownSeconds - DeltaTime, 0.0f);
		float RemainingCooldownPercent = FMath::Clamp(RemainingCooldownSeconds <= 0.0f ? 1.0f : 1.0f - RemainingCooldownSeconds / SpawnCooldown, 0.0f, 1.0f);
		
		OnRemainingCooldownChanged.Broadcast(RemainingCooldownSeconds, RemainingCooldownPercent);
	}
}