// Copyright Surround, Inc. All Rights Reserved.


#include "SMUserWidget_VictoryDefeat.h"

#include "AbilitySystemComponent.h"
// #include "Animation/WidgetAnimation.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"
#include "Games/SMGameState.h"
#include "Interfaces/SMTeamInterface.h"
#include "Utilities/SMLog.h"

void USMUserWidget_VictoryDefeat::NativeConstruct()
{
	Super::NativeConstruct();

	Panels.Add(EVictoryDefeatResult::Victory, VictoryPanel);
	Panels.Add(EVictoryDefeatResult::Defeat, DefeatPanel);
	Panels.Add(EVictoryDefeatResult::Draw, DrawPanel);

	TeamPanels.Add(EVictoryDefeatResult::Victory);
	TeamPanels[EVictoryDefeatResult::Victory].Add(ESMTeam::EDM, VictoryBackgroundPanel_EDM);
	TeamPanels[EVictoryDefeatResult::Victory].Add(ESMTeam::FutureBass, VictoryBackgroundPanel_FB);

	WidgetAnimations.Add(EVictoryDefeatResult::Victory);
	WidgetAnimations[EVictoryDefeatResult::Victory].Add(ESMTeam::EDM, Victory_Ani_EDM);
	WidgetAnimations[EVictoryDefeatResult::Victory].Add(ESMTeam::FutureBass, Victory_Ani_FB);

	WidgetAnimations.Add(EVictoryDefeatResult::Defeat);
	WidgetAnimations[EVictoryDefeatResult::Defeat].Add(ESMTeam::None, Defeat_Ani);

	WidgetAnimations.Add(EVictoryDefeatResult::Draw);
	WidgetAnimations[EVictoryDefeatResult::Draw].Add(ESMTeam::None, Draw_Ani);
}

void USMUserWidget_VictoryDefeat::SetASC(UAbilitySystemComponent* InASC)
{
	Super::SetASC(InASC);

	if (!ensureAlways(InASC))
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
		SMGameState->OnEndRound.AddUObject(this, &ThisClass::OnEndRound);
	}
	else
	{
		// 만약 바인드에 실패하면 계속 시도합니다.
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::BindToGameState);
	}
}

void USMUserWidget_VictoryDefeat::OnEndRound(ESMTeam VictoryTeam)
{
	ISMTeamInterface* SourceTeamInterface = Cast<ISMTeamInterface>(ASC->GetAvatarActor());
	if (!ensure(SourceTeamInterface))
	{
		return;
	}
	const ESMTeam SourceTeam = SourceTeamInterface->GetTeam();

	EVictoryDefeatResult VictoryDefeatResult;
	if (VictoryTeam == ESMTeam::None)
	{
		VictoryDefeatResult = EVictoryDefeatResult::Draw;
	}
	else if (VictoryTeam == SourceTeam)
	{
		VictoryDefeatResult = EVictoryDefeatResult::Victory;
	}
	else
	{
		VictoryDefeatResult = EVictoryDefeatResult::Defeat;
	}

	ShowResult(VictoryDefeatResult, SourceTeam);
	PlayResultAnimation(VictoryDefeatResult, SourceTeam);
}

void USMUserWidget_VictoryDefeat::ShowResult(EVictoryDefeatResult InResult, ESMTeam InSourceTeam)
{
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	if (Panels.Find(InResult))
	{
		Panels[InResult]->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}

	if (TeamPanels.Find(InResult))
	{
		if (TeamPanels[InResult].Find(InSourceTeam))
		{
			TeamPanels[InResult][InSourceTeam]->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
	}
}

void USMUserWidget_VictoryDefeat::PlayResultAnimation(EVictoryDefeatResult InResult, ESMTeam InSourceTeam)
{
	if (WidgetAnimations.Find(InResult))
	{
		// 팀이 없는 애니메이션이 있다면 재생합니다.
		if (WidgetAnimations[InResult].Find(ESMTeam::None))
		{
			PlayAnimation(WidgetAnimations[InResult][ESMTeam::None]);
		}

		// 팀 전용 애니메이션도 있다면 재생합니다.
		if (WidgetAnimations[InResult].Find(InSourceTeam))
		{
			UWidgetAnimation* CachedWidgetAnimation = WidgetAnimations[InResult][InSourceTeam];
			PlayAnimation(CachedWidgetAnimation);
		}
	}
}
