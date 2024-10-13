// Copyright Studio Surround. All Rights Reserved.


#include "SMTutorialInvisibleWallBase.h"

#include "Components/BoxComponent.h"
#include "Utilities/SMCollision.h"


ASMTutorialInvisibleWallBase::ASMTutorialInvisibleWallBase()
{
	PrimaryActorTick.bCanEverTick = true;

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	RootComponent = RootSceneComponent;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetupAttachment(RootComponent);
	BoxComponent->SetCollisionProfileName(SMCollisionProfileName::InvisibleWallDynamic);
	BoxComponent->SetRelativeLocation(FVector(50.0));
	BoxComponent->InitBoxExtent(FVector(50.0));
}
