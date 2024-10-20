// Copyright Studio Surround. All Rights Reserved.


#include "SMTutorialLocation.h"


ASMTutorialLocation::ASMTutorialLocation()
{
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
}