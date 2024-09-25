#include "SMThrowItem.h"

#include "Items/SMThrowableItem.h"
#include "FunctionLibraries/SMCalculateBlueprintLibrary.h"
#include "FunctionLibraries/SMTileFunctionLibrary.h"
#include "Utilities/SMLog.h"

ASMThrowItem::ASMThrowItem()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;
}

void ASMThrowItem::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	const float TileSize = USMTileFunctionLibrary::DefaultTileSize;
	const float CenterOffset = TileSize / 2.0f;

	FVector HalfExtent;
	HalfExtent.X = (MaxThrowTilesColumn * TileSize) + CenterOffset;
	HalfExtent.Y = (MaxThrowTilesRow * TileSize) + CenterOffset;
	HalfExtent.Z = ParabolaHeight / 2.0f;

	const FVector SourceLocation = GetActorLocation();
	const FVector BoxCenter = FVector(SourceLocation.X, SourceLocation.Y, SourceLocation.Z + HalfExtent.Z);

	DrawDebugBox(GetWorld(), BoxCenter, HalfExtent, FQuat::Identity, FColor::Red, false, -1, 0, 5.0f);
}

void ASMThrowItem::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		GetWorld()->GetTimerManager().SetTimer(ThrowTimerHandle, this, &ASMThrowItem::ThrowItem, ThrowInterval, true);
	}
}

void ASMThrowItem::ThrowItem()
{
	for (int i = 0; i < ThrowCount; ++i)
	{
		InternalThrowItem();
	}
}

void ASMThrowItem::InternalThrowItem()
{
	const float TileSize = USMTileFunctionLibrary::DefaultTileSize;
	const float CenterOffset = TileSize / 2.0f;

	const float XRange = (MaxThrowTilesColumn * TileSize) + CenterOffset;
	const float RandomX = FMath::RandRange(-XRange, XRange - TileSize);

	const float YRange = (MaxThrowTilesRow * TileSize) + CenterOffset;
	const float RandomY = FMath::RandRange(-YRange, YRange - TileSize);

	const FVector SpawnLocation = GetActorLocation();

	FVector TargetLocation;
	TargetLocation.X = FMath::GridSnap(SpawnLocation.X + RandomX, TileSize) + (TileSize / 2.0f);
	TargetLocation.Y = FMath::GridSnap(SpawnLocation.Y + RandomY, TileSize) + (TileSize / 2.0f);
	TargetLocation.Z = SpawnLocation.Z;

	FHitResult HitResult;

	const float ObstacleHeightThreshold = 1000.0f;
	const FVector Offset(0.0f, 0.0f, ObstacleHeightThreshold);
	const FVector Start = TargetLocation + Offset;
	const FVector End = TargetLocation;

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams))
	{
		return InternalThrowItem();
	}

	const FRotator SpawnRotation = GetActorRotation();

	ASMThrowableItem* ThrowableItem = GetWorld()->SpawnActor<ASMThrowableItem>(ItemToThrow, SpawnLocation, SpawnRotation);
	if (ThrowableItem)
	{
		float RandomGravity = FMath::RandRange(-2000.0f, -500.0f);
		FVector LaunchVelocity = USMCalculateBlueprintLibrary::SuggestProjectileVelocity_CustomApexHeight(this, SpawnLocation, TargetLocation, ParabolaHeight, RandomGravity);

		ThrowableItem->SetThrowItem(LaunchVelocity, SpawnLocation, TargetLocation, RandomGravity);
	}
}
