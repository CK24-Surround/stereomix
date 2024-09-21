// Copyright Studio Surround. All Rights Reserved.


#include "SMHoldableItemBase.h"


// Sets default values for this component's properties
USMHoldableItemBase::USMHoldableItemBase()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void USMHoldableItemBase::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void USMHoldableItemBase::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

