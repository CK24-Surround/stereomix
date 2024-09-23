#include "SMThrowItem.h"

#include "Items/SMThrowableItem.h"
#include "FunctionLibraries/SMCalculateBlueprintLibrary.h"
#include "Utilities/SMLog.h"

ASMThrowItem::ASMThrowItem()
{
    PrimaryActorTick.bCanEverTick = true;
	ThrowCount = 1;
    ThrowInterval = 5.0f;
	TileSize = 150.0f;
	MaxThrowTilesRow = 5;
	MaxThrowTilesColumn = 5;
    ParabolaHeight = 1000.0f;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;
}

void ASMThrowItem::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	FVector BoxCenter = GetActorLocation();
	FVector BoxExtent(MaxThrowTilesColumn * TileSize, MaxThrowTilesRow * TileSize, ParabolaHeight / 2.0f);

	BoxCenter.Z += BoxExtent.Z;
	
	DrawDebugBox(GetWorld(), BoxCenter, BoxExtent, FQuat::Identity, FColor::Red, false, -1, 0, 5.0f);
}

void ASMThrowItem::BeginPlay()
{
    Super::BeginPlay();

    if (HasAuthority())
    {
        GetWorld()->GetTimerManager().SetTimer(ThrowTimerHandle, this, &ASMThrowItem::ServerThrowItem, ThrowInterval, true);
    }
}

void ASMThrowItem::ServerThrowItem_Implementation()
{
	for (int i = 0; i < ThrowCount; ++i)
	{
		ThrowItem();
	}
}

void ASMThrowItem::ThrowItem()
{
	FVector SpawnLocation = GetActorLocation();
	
	float XRange = MaxThrowTilesColumn * TileSize;
	float YRange = MaxThrowTilesRow * TileSize;
	float RandomX = FMath::RandRange(-XRange, XRange - TileSize);
	float RandomY = FMath::RandRange(-YRange, YRange - TileSize);
		
	FVector TargetLocation = FVector(
		FMath::GridSnap(SpawnLocation.X + RandomX, TileSize) + (TileSize / 2.0f),
		FMath::GridSnap(SpawnLocation.Y + RandomY, TileSize) + (TileSize / 2.0f),
		GetActorLocation().Z);

	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	bool bIsBlocked = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		TargetLocation + FVector(0.0f, 0.0f, 1000.0f),
		TargetLocation,
		ECC_Visibility,
		CollisionParams);

	if (bIsBlocked)
	{
		return ThrowItem();
	}
		
	FRotator SpawnRotation = GetActorRotation();

	float RandomGravity = FMath::RandRange(-2000.0f, -500.0f);
	FVector LaunchVelocity = USMCalculateBlueprintLibrary::SuggestProjectileVelocity_CustomApexHeight(this, SpawnLocation, TargetLocation, ParabolaHeight, RandomGravity);

	ASMThrowableItem* ThrowableItem = GetWorld()->SpawnActor<ASMThrowableItem>(ItemToThrow, SpawnLocation, SpawnRotation);
	if (ThrowableItem)
	{
		ThrowableItem->SetThrowItem(LaunchVelocity, SpawnLocation, TargetLocation, RandomGravity);
	}
}
