#include "SMThrowItem.h"

#include "Net/UnrealNetwork.h"
#include "FunctionLibraries/SMCalculateBlueprintLibrary.h"
#include "Items/HoldableItem/SMHoldableItemBase.h"

ASMThrowItem::ASMThrowItem()
{
    PrimaryActorTick.bCanEverTick = true;
    ParabolaHeight = 500.0f;
    LandingTime = 2.0f;
    ThrowInterval = 5.0f;

    bReplicates = true;
    bAlwaysRelevant = true;
}

void ASMThrowItem::BeginPlay()
{
    Super::BeginPlay();

    if (HasAuthority())  // 서버에서만 실행
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
		FVector TargetLocation = FVector(SpawnLocation.X + RandomX, SpawnLocation.Y + RandomY, GetActorLocation().Z);

		FRotator SpawnRotation = GetActorRotation();

		// 아이템 스폰
		SpawnedItem.Add(GetWorld()->SpawnActor<ASMHoldableItemBase>(ItemToThrow, SpawnLocation, SpawnRotation));

		InitialLocation.Add(SpawnLocation);
		
		float GravityZ = -980.0f;
		LaunchVelocity.Add(USMCalculateBlueprintLibrary::SuggestProjectileVelocity_CustomApexHeight(this, SpawnLocation, TargetLocation, ParabolaHeight, GravityZ));
		
		ThrowStartTime.Add(GetWorld()->GetTimeSeconds());
	}
}

void ASMThrowItem::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

	for (int i = 0; i < SpawnedItem.Num(); ++i)
	{
		float CurrentTime = GetWorld()->GetTimeSeconds();
		float ElapsedTime = CurrentTime - ThrowStartTime[i];

		float x = LaunchVelocity[i].X * ElapsedTime;
		float y = LaunchVelocity[i].Y * ElapsedTime;
		float Z = LaunchVelocity[i].Z * ElapsedTime + 0.5f * -980.0f * FMath::Pow(ElapsedTime, 2);
		FVector NewLocation = InitialLocation[i] + FVector(x, y, Z);

		SpawnedItem[i]->SetActorLocation(NewLocation);
	}

	for (int i = 0; i < SpawnedItem.Num(); ++i)
	{
		if (SpawnedItem[i]->GetActorLocation().Z < GetActorLocation().Z)
		{
			SpawnedItem.RemoveAt(i);
			LaunchVelocity.RemoveAt(i);
			InitialLocation.RemoveAt(i);
			ThrowStartTime.RemoveAt(i);
		}
	}
}

// 복제 설정
void ASMThrowItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ASMThrowItem, SpawnedItem);
    DOREPLIFETIME(ASMThrowItem, LaunchVelocity);
    DOREPLIFETIME(ASMThrowItem, InitialLocation);
    DOREPLIFETIME(ASMThrowItem, ThrowStartTime);
}
