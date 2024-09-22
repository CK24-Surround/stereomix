#include "SMThrowItem.h"

#include "Items/SMThrowableItem.h"
#include "FunctionLibraries/SMCalculateBlueprintLibrary.h"

ASMThrowItem::ASMThrowItem()
{
    PrimaryActorTick.bCanEverTick = true;
    ParabolaHeight = 500.0f;
    LandingTime = 2.0f;
    ThrowInterval = 5.0f;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;
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
	FVector SpawnLocation = GetActorLocation();
	
	for (int i = 0; i < 3; ++i)
	{
		float RandomX = FMath::RandRange(-1000.0f, 1000.0f);
		float RandomY = FMath::RandRange(-1000.0f, 1000.0f);
		
		FVector TargetLocation = FVector(
			FMath::GridSnap(SpawnLocation.X + RandomX, 150.0f) - 75.0f,
			FMath::GridSnap(SpawnLocation.Y + RandomY, 150.0f) - 75.0f,
			GetActorLocation().Z);
		
		FRotator SpawnRotation = GetActorRotation();
        
		FVector LaunchVelocity = USMCalculateBlueprintLibrary::SuggestProjectileVelocity_CustomApexHeight(this, SpawnLocation, TargetLocation, ParabolaHeight, -980.0f);

		ASMThrowableItem* ThrowableItem = GetWorld()->SpawnActor<ASMThrowableItem>(ItemToThrow, SpawnLocation, SpawnRotation);
		if (ThrowableItem)
		{
			ThrowableItem->SetThrowItem(LaunchVelocity, SpawnLocation, TargetLocation);
		}
	}
}
