// Copyright Surround, Inc. All Rights Reserved.


#include "SMHoldInteractionComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
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

void USMHoldInteractionComponent::SetActorHoldingMe(AActor* NewActorHoldingMe)
{
	if (GetOwnerRole() != ROLE_Authority)
	{
		NET_LOG(SourceActor, Warning, TEXT("서버에서만 호출되야합니다."));
		return;
	}

	if (HoldingMeActor != NewActorHoldingMe)
	{
		HoldingMeActor = NewActorHoldingMe;
		OnRep_HoldingMeActor();
		SourceActor->ForceNetUpdate();
	}
}

void USMHoldInteractionComponent::OnRep_HoldingMeActor()
{
	USMAbilitySystemComponent* SourceASC = Cast<USMAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(SourceActor));

	if (HoldingMeActor.Get())
	{
		if (SourceASC && GetOwnerRole() == ROLE_Authority)
		{
			SourceASC->AddTag(SMTags::Character::State::Holded);
		}

		OnHoldedStateEntry.Broadcast();
	}
	else
	{
		if (SourceASC && GetOwnerRole() == ROLE_Authority)
		{
			SourceASC->RemoveTag(SMTags::Character::State::Holded);
		}

		OnHoldedStateExit.Broadcast();
	}
}
