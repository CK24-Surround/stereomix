// Copyright Surround, Inc. All Rights Reserved.


#include "SMUserWidget_VictoryDefeat.h"

#include "AbilitySystemComponent.h"
#include "Components/Image.h"
#include "Games/SMGameState.h"
#include "Interfaces/SMTeamInterface.h"
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

	ISMTeamInterface* SourceTeamInterface = Cast<ISMTeamInterface>(ASC->GetAvatarActor());
	if (!ensure(SourceTeamInterface))
	{
		return;
	}

	const ESMTeam SourceTeam = SourceTeamInterface->GetTeam();

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
	Victory->SetVisibility(ESlateVisibility::HitTestInvisible);
}

void USMUserWidget_VictoryDefeat::ShowDefeat()
{
	Defeat->SetVisibility(ESlateVisibility::HitTestInvisible);
}

void USMUserWidget_VictoryDefeat::ShowDraw()
{
	Draw->SetVisibility(ESlateVisibility::HitTestInvisible);
}
