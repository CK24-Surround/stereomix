// Copyright Surround, Inc. All Rights Reserved.


#include "SMHoldInteractionComponent.h"

#include "Net/UnrealNetwork.h"
#include "Utilities/SMLog.h"


USMHoldInteractionComponent::USMHoldInteractionComponent()
{
	SetIsReplicatedByDefault(true);
}

void USMHoldInteractionComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, HoldingMeActor);
}

void USMHoldInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	SourceActor = GetOwner();
	ensureAlways(SourceActor);
}

void USMHoldInteractionComponent::SetActorHoldingMe(AActor* NewActorCatchingMe)
{
	if (GetOwnerRole() != ROLE_Authority)
	{
		NET_LOG(SourceActor, Warning, TEXT("서버에서만 호출되야합니다."));
		return;
	}

	if (HoldingMeActor != NewActorCatchingMe)
	{
		HoldingMeActor = NewActorCatchingMe;
	}
}
