// Copyright Studio Surround. All Rights Reserved.


#include "SMHIC_OverlapItem.h"


USMHIC_OverlapItem::USMHIC_OverlapItem()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USMHIC_OverlapItem::BeginPlay()
{
	Super::BeginPlay();
}

void USMHIC_OverlapItem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
