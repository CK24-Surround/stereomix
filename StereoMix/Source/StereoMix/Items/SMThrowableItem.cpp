// Copyright Studio Surround. All Rights Reserved.


#include "SMThrowableItem.h"

#include "NiagaraComponent.h"
#include "HoldableItem/SMHoldableItemBase.h"
#include "Net/UnrealNetwork.h"


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
	float Z = LaunchVelocity.Z * ElapsedTime + 0.5f * -980.0f * FMath::Pow(ElapsedTime, 2);
	FVector NewLocation = InitialLocation + FVector(x, y, Z);
	
	SetActorLocation(NewLocation);
	
	if (NewLocation.Z < TargetLocation.Z)
	{
		bEnableThrow = false;
		if (Item)
		{
			FRotator SpawnRotation = GetActorRotation();
			GetWorld()->SpawnActor<ASMHoldableItemBase>(Item, TargetLocation, SpawnRotation);
		}
		Destroy();
	}
}

void ASMThrowableItem::SetThrowItem(const FVector& InLaunchVelocity, const FVector& InInitialLocation, const FVector& InTargetLocation)
{
	bEnableThrow = true;
	ThrowStartTime = GetWorld()->GetTimeSeconds();
	LaunchVelocity = InLaunchVelocity;
	InitialLocation = InInitialLocation;
	TargetLocation = InTargetLocation;
}

void ASMThrowableItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASMThrowableItem, bEnableThrow);
	DOREPLIFETIME(ASMThrowableItem, ThrowStartTime);
	DOREPLIFETIME(ASMThrowableItem, LaunchVelocity);
	DOREPLIFETIME(ASMThrowableItem, InitialLocation);
	DOREPLIFETIME(ASMThrowableItem, TargetLocation);
}
