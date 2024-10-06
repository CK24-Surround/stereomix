// Copyright Studio Surround. All Rights Reserved.


#include "SMTutorialManagerComponent.h"

#include "Utilities/SMLog.h"


USMTutorialManagerComponent::USMTutorialManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USMTutorialManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	NET_VLOG(GetOwner(), INDEX_NONE, 3.0f, TEXT("\n따르릉! 따르릉!\n뮤지션님! 지금 어디세요? 이러다가 공연에 늦겠어요!"));
}
