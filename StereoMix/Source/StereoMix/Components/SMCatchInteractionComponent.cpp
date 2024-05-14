// Copyright Surround, Inc. All Rights Reserved.


#include "SMCatchInteractionComponent.h"

#include "Net/UnrealNetwork.h"
#include "Utilities/SMLog.h"


USMCatchInteractionComponent::USMCatchInteractionComponent()
{
	SetIsReplicatedByDefault(true);
}

void USMCatchInteractionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, CatchingMeActor);
}

void USMCatchInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	SourceActor = GetOwner();
	check(SourceActor);
}

void USMCatchInteractionComponent::SetActorCatchingMe(AActor* NewActorCatchingMe)
{
	if (GetOwnerRole() != ROLE_Authority)
	{
		NET_LOG(SourceActor, Warning, TEXT("서버에서만 호출되야합니다."));
		return;
	}

	if (CatchingMeActor != NewActorCatchingMe)
	{
		CatchingMeActor = NewActorCatchingMe;
		OnRepCatchingMeActor();
	}
}

void USMCatchInteractionComponent::OnRepCatchingMeActor()
{
	// 나를 잡은 대상이 nullptr이 아닌 경우라면 잡힌 상태로 해당 이벤트를 브로드캐스트합니다.
	if (CatchingMeActor != nullptr)
	{
		OnStartCaught.Broadcast();
	}
}
