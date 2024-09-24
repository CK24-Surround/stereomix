// Copyright Studio Surround. All Rights Reserved.


#include "SMThrowableItem.h"

#include "NiagaraComponent.h"
#include "HoldableItem/SMHoldableItemBase.h"


ASMThrowableItem::ASMThrowableItem()
{
	bReplicates = true;
	Super::SetReplicateMovement(true);
	
	PrimaryActorTick.bCanEverTick = true;
	bEnableThrow = false;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;

	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	NiagaraComponent->SetupAttachment(RootComponent);

	DestroyTime = 15.0f;
}

void ASMThrowableItem::BeginPlay()
{
	Super::BeginPlay();
}

void ASMThrowableItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!HasAuthority())
	{
		return;
	}

	if (!bEnableThrow)
	{
		return;
	}
	
	float CurrentTime = GetWorld()->GetTimeSeconds();
	float ElapsedTime = CurrentTime - ThrowStartTime;

	float x = LaunchVelocity.X * ElapsedTime;
	float y = LaunchVelocity.Y * ElapsedTime;
	float Z = LaunchVelocity.Z * ElapsedTime + 0.5f * ThrowGravity * FMath::Pow(ElapsedTime, 2);
	FVector NewLocation = InitialLocation + FVector(x, y, Z);
	
	SetActorLocation(NewLocation);
	
	if (NewLocation.Z < TargetLocation.Z)
	{
		bEnableThrow = false;
		
		if (!ItemToSpawn)
		{
			Destroy();
			return;
		}

		FRotator SpawnRotation = GetActorRotation();
		ASMHoldableItemBase* SpawnedItem = GetWorld()->SpawnActor<ASMHoldableItemBase>(ItemToSpawn, TargetLocation, SpawnRotation);
		if (SpawnedItem && DestroyTime > 0.0f)
		{
			TWeakObjectPtr<ASMHoldableItemBase> WeakSpawnedItem(SpawnedItem);

			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, [WeakSpawnedItem]()
			{
				if (WeakSpawnedItem.IsValid())
				{
					WeakSpawnedItem->Destroy();
				}
			}, DestroyTime, false);
		}
		
		Destroy();
	}
}

void ASMThrowableItem::SetThrowItem(const FVector& InLaunchVelocity, const FVector& InInitialLocation, const FVector& InTargetLocation, const float& InGravity)
{
	bEnableThrow = true;
	ThrowStartTime = GetWorld()->GetTimeSeconds();
	LaunchVelocity = InLaunchVelocity;
	InitialLocation = InInitialLocation;
	TargetLocation = InTargetLocation;
	ThrowGravity = InGravity;
}
