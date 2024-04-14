// Fill out your copyright notice in the Description page of Project Settings.


#include "SMTeamComponent.h"

#include "Net/UnrealNetwork.h"


USMTeamComponent::USMTeamComponent()
{
	SetIsReplicatedByDefault(true);

	Team = ESMTeam::None;
}

void USMTeamComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USMTeamComponent, Team);
}

void USMTeamComponent::SetTeam(ESMTeam InTeam)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		Team = InTeam;
		OnRep_Team();
	}
}

void USMTeamComponent::OnRep_Team()
{
	OnChangeTeam.Broadcast();
}
