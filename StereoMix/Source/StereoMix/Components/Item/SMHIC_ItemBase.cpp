// Copyright Studio Surround. All Rights Reserved.


#include "SMHIC_ItemBase.h"


USMHIC_ItemBase::USMHIC_ItemBase()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USMHIC_ItemBase::BeginPlay()
{
	Super::BeginPlay();
}

void USMHIC_ItemBase::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
