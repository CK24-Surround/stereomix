// Copyright Studio Surround. All Rights Reserved.


#include "SMTutorialWall.h"

#include "Utilities/SMCollision.h"


ASMTutorialWall::ASMTutorialWall()
{
	PrimaryActorTick.bCanEverTick = true;

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	RootComponent = RootSceneComponent;

	WallComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WallComponent"));
	WallComponent->SetupAttachment(RootComponent);
	WallComponent->SetCollisionProfileName(SMCollisionProfileName::BlockAll);
	WallComponent->SetRelativeLocationAndRotation(FVector(0.0, 0.0, 150.0), FRotator(90.0, 0.0, 0.0));
}
