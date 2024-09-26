// Copyright Studio Surround. All Rights Reserved.


#include "SMThrowableItem.h"

#include "NiagaraComponent.h"
#include "SMItemBase.h"
#include "Utilities/SMCollision.h"


ASMThrowableItem::ASMThrowableItem()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	Super::SetReplicateMovement(true);

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;

	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	NiagaraComponent->SetupAttachment(RootComponent);
	NiagaraComponent->SetCollisionProfileName(SMCollisionProfileName::NoCollision);
}

void ASMThrowableItem::SetSpawnItem(TSubclassOf<ASMItemBase> InItemToSpawn)
{
	ItemToSpawn = InItemToSpawn;
}

void ASMThrowableItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!HasAuthority() || !bEnableThrow)
	{
		return;
	}

	const float CurrentTime = GetWorld()->GetTimeSeconds();
	const float ElapsedTime = CurrentTime - ThrowStartTime;

	FVector DeltaLocation;
	DeltaLocation.X = LaunchVelocity.X * ElapsedTime;
	DeltaLocation.Y = LaunchVelocity.Y * ElapsedTime;
	DeltaLocation.Z = LaunchVelocity.Z * ElapsedTime + 0.5f * ThrowGravity * FMath::Pow(ElapsedTime, 2);

	const FVector NewLocation = InitialLocation + DeltaLocation;
	SetActorLocation(NewLocation);

	if (NewLocation.Z < TargetLocation.Z)
	{
		OnTargetArrived();
	}
}

void ASMThrowableItem::OnTargetArrived()
{
	bEnableThrow = false;

	if (!ItemToSpawn)
	{
		Destroy();
		return;
	}

	const FRotator SpawnRotation = GetActorRotation();
	ASMItemBase* SpawnedItem = GetWorld()->SpawnActor<ASMItemBase>(ItemToSpawn, TargetLocation, SpawnRotation);
	if (SpawnedItem && DestroyTime > 0.0f)
	{
		TWeakObjectPtr<ASMItemBase> WeakSpawnedItem(SpawnedItem);

		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [WeakSpawnedItem]() {
			if (WeakSpawnedItem.IsValid())
			{
				WeakSpawnedItem->Destroy();
			}
		}, DestroyTime, false);
	}

	Destroy();
}

void ASMThrowableItem::SetAttribute(const FVector& InLaunchVelocity, const FVector& InInitialLocation, const FVector& InTargetLocation, const float& InGravity)
{
	bEnableThrow = true;
	ThrowStartTime = GetWorld()->GetTimeSeconds();
	LaunchVelocity = InLaunchVelocity;
	InitialLocation = InInitialLocation;
	TargetLocation = InTargetLocation;
	ThrowGravity = InGravity;
}
