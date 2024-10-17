#include "SMThrowItem.h"
#include "StereoMixLog.h"
#include "Actors/Items/SMThrowableItem.h"
#include "Actors/Tiles/SMTile.h"
#include "FunctionLibraries/SMCalculateBlueprintLibrary.h"
#include "FunctionLibraries/SMTileFunctionLibrary.h"

ASMThrowItem::ASMThrowItem()
{
    PrimaryActorTick.bCanEverTick = true;

    SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
    RootComponent = SceneComponent;
}

void ASMThrowItem::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);

    const FVector HalfExtent = CalculateHalfExtent(MaxThrowTilesColumn, MaxThrowTilesRow, ParabolaHeight);
    const FVector BoxCenter = GetBoxCenter(GetActorLocation(), HalfExtent.Z);

    DrawDebugBox(GetWorld(), BoxCenter, HalfExtent, FQuat::Identity, FColor::Red, false, 0.1f, 0, 5.0f);
}

void ASMThrowItem::BeginPlay()
{
    Super::BeginPlay();

    if (HasAuthority())
    {
        InitializeAvailableSpawnLocations();
    	UE_LOG(LogStereoMix, Log, TEXT("Available Throwable Item Spawn Locations: %d"), AvailableSpawnLocations.Num());

        if (AvailableSpawnLocations.Num() > 0)
        {
            ScheduleThrowItem();
        }
    }
}

void ASMThrowItem::InitializeAvailableSpawnLocations()
{
	const UWorld* World = GetWorld();
    const FVector HalfExtent = CalculateHalfExtent(MaxThrowTilesColumn, MaxThrowTilesRow, ParabolaHeight);
    const FVector BoxCenter = GetActorLocation();

	for (TArray<ASMTile*> TilesInSpawnArea = USMTileFunctionLibrary::GetTilesInBox(World, BoxCenter, HalfExtent); const ASMTile* Tile : TilesInSpawnArea)
    {
	    if (FVector SpawnLocation = CalculateSpawnLocation(Tile->GetActorLocation(), BoxCenter.Z); IsLocationAvailableForSpawn(SpawnLocation))
        {
            AvailableSpawnLocations.Add(SpawnLocation);
        }
    }
}

FVector ASMThrowItem::CalculateHalfExtent(const int32 Columns, const int32 Rows, const float Height)
{
	constexpr float TileSize = USMTileFunctionLibrary::DefaultTileSize;
    return FVector(Columns * TileSize, Rows * TileSize, Height / 2.0f);
}

FVector ASMThrowItem::GetBoxCenter(const FVector& ActorLocation, const float ZOffset)
{
    return FVector(ActorLocation.X, ActorLocation.Y, ActorLocation.Z + ZOffset);
}

FVector ASMThrowItem::CalculateSpawnLocation(const FVector& TileLocation, const float Z)
{
	constexpr float TileSize = USMTileFunctionLibrary::DefaultTileSize;
    return FVector(
        FMath::GridSnap(TileLocation.X, TileSize) + TileSize / 2.0f,
        FMath::GridSnap(TileLocation.Y, TileSize) + TileSize / 2.0f,
        Z
    );
}

bool ASMThrowItem::IsLocationAvailableForSpawn(const FVector& Location) const
{
	constexpr float ObstacleHeightThreshold = 1000.0f;
    const FVector Start = Location + FVector(0.0f, 0.0f, ObstacleHeightThreshold);
    const FVector End = Location;

    FHitResult HitResult;
    FCollisionQueryParams CollisionParams;
    CollisionParams.AddIgnoredActor(this);

    return !GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams);
}

void ASMThrowItem::ScheduleThrowItem()
{
	if (const UWorld* World = GetWorld())
	{
	    FTimerHandle ThrowTimerHandle;
	    World->GetTimerManager().SetTimer(ThrowTimerHandle, [ThisWeakPtr = MakeWeakObjectPtr(this)] {
	        if (ThisWeakPtr.IsValid())
	        {
	            ThisWeakPtr->ThrowItem();
	        }
	    }, ThrowInterval, true);
	}
}

void ASMThrowItem::ThrowItem()
{
    const int32 ValidThrowCount = FMath::Min(ThrowCount, AvailableSpawnLocations.Num());
    TArray<FVector> SelectedLocations = AvailableSpawnLocations;

    for (int32 i = 0; i < ValidThrowCount; ++i)
    {
        const int32 RandomIndex = FMath::RandRange(0, SelectedLocations.Num() - 1);
        FVector TargetLocation = SelectedLocations[RandomIndex];
        SelectedLocations.RemoveAt(RandomIndex);

        InternalThrowItem(TargetLocation);
    }
}

void ASMThrowItem::InternalThrowItem(const FVector& TargetLocation)
{
    const FVector SpawnLocation = GetActorLocation();
    const FRotator SpawnRotation = GetActorRotation();

    if (ASMThrowableItem* ThrowableItem = GetWorld()->SpawnActor<ASMThrowableItem>(ThrowableItemClass, SpawnLocation, SpawnRotation))
    {
        const float RandomGravity = FMath::RandRange(-2000.0f, -500.0f);
        const FVector LaunchVelocity = USMCalculateBlueprintLibrary::SuggestProjectileVelocity_CustomApexHeight(
            this, SpawnLocation, TargetLocation, ParabolaHeight, RandomGravity
        );

        const TSubclassOf<ASMItemBase> RandomItem = ThrowingItems[FMath::RandRange(0, ThrowingItems.Num() - 1)];

        ThrowableItem->SetSpawnItem(RandomItem);
        ThrowableItem->SetAttribute(LaunchVelocity, SpawnLocation, TargetLocation, RandomGravity);
    }
}
