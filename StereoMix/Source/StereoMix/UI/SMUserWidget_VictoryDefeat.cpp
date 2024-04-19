// Fill out your copyright notice in the Description page of Project Settings.


#include "SMUserWidget_VictoryDefeat.h"

#include "AbilitySystemComponent.h"
#include "Components/SMTeamComponent.h"
#include "Games/SMGameState.h"
#include "Interfaces/SMTeamComponentInterface.h"
#include "Utilities/SMLog.h"

void USMUserWidget_VictoryDefeat::SetASC(UAbilitySystemComponent* InASC)
{
	Super::SetASC(InASC);

	if (!ensure(InASC))
	{
		return;
	}

	BindToGameState();
}

void USMUserWidget_VictoryDefeat::BindToGameState()
{
	ASMGameState* SMGameState = GetWorld()->GetGameState<ASMGameState>();
	if (SMGameState)
	{
		SMGameState->OnEndRound.AddUObject(this, &USMUserWidget_VictoryDefeat::OnEndRound);
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &USMUserWidget_VictoryDefeat::BindToGameState);
	}
}

void USMUserWidget_VictoryDefeat::OnEndRound(ESMTeam VictoryTeam)
{
	if (VictoryTeam == ESMTeam::None)
	{
		ShowDraw();
		return;
	}
	
	ISMTeamComponentInterface* SourceTeamComponentInterface = Cast<ISMTeamComponentInterface>(ASC->GetAvatarActor());
	if (!ensure(SourceTeamComponentInterface))
	{
		return;
	}

	USMTeamComponent* SourceTeamComponent = SourceTeamComponentInterface->GetTeamComponent();
	if (!ensure(SourceTeamComponent))
	{
		return;
	}

	const ESMTeam SourceTeam = SourceTeamComponent->GetTeam();
	
	if (SourceTeam == VictoryTeam)
	{
		ShowVictory();
	}
	else
	{
		ShowDefeat();
	}
}

void USMUserWidget_VictoryDefeat::ShowVictory()
{
	NET_LOG(ASC->GetAvatarActor(), Warning, TEXT("승리"));
}
void USMUserWidget_VictoryDefeat::ShowDefeat()
{
	NET_LOG(ASC->GetAvatarActor(), Warning, TEXT("패배"));
}
void USMUserWidget_VictoryDefeat::ShowDraw()
{
	NET_LOG(ASC->GetAvatarActor(), Warning, TEXT("무승부"));
}
