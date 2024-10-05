// Copyright Studio Surround. All Rights Reserved.


#include "SMScoreManagerComponent.h"


USMScoreManagerComponent::USMScoreManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USMScoreManagerComponent::BeginPlay()
{
	Super::BeginPlay();
}

void USMScoreManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

