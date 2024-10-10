// Copyright Studio Surround. All Rights Reserved.


#include "SMHIC_TrainingDummy.h"


USMHIC_TrainingDummy::USMHIC_TrainingDummy()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USMHIC_TrainingDummy::BeginPlay()
{
	Super::BeginPlay();
}

void USMHIC_TrainingDummy::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

