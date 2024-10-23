// Copyright Studio Surround. All Rights Reserved.


#include "SMCharacterSelectorProfile.h"

#include "CommonTextBlock.h"
#include "Components/Image.h"
#include "Games/SMPlayerState.h"

class ASMPlayerState;

void USMCharacterSelectorProfile::SetProfileImage(ESMTeam Team, ESMCharacterType CharacterType)
{
	(Team == ESMTeam::EDM ? ProfileEDM1 : ProfileFB1)->SetVisibility(ESlateVisibility::Hidden);
	(Team == ESMTeam::EDM ? ProfileEDM2 : ProfileFB2)->SetVisibility(ESlateVisibility::Hidden);
	(Team == ESMTeam::EDM ? ProfileEDM3 : ProfileFB3)->SetVisibility(ESlateVisibility::Hidden);

	switch (CharacterType)
	{
		case ESMCharacterType::None:
			PlayerCharacterType->SetText(FText::FromString(TEXT("선택중...")));
			break;
		case ESMCharacterType::ElectricGuitar:
			PlayerCharacterType->SetText(FText::FromString(TEXT("일렉기타")));
			(Team == ESMTeam::EDM ? ProfileEDM1 : ProfileFB1)->SetVisibility(ESlateVisibility::Visible);
			break;
		case ESMCharacterType::Piano:
			PlayerCharacterType->SetText(FText::FromString(TEXT("피아노")));
			(Team == ESMTeam::EDM ? ProfileEDM2 : ProfileFB2)->SetVisibility(ESlateVisibility::Visible);
			break;
		case ESMCharacterType::Bass:
			PlayerCharacterType->SetText(FText::FromString(TEXT("베이스")));
			(Team == ESMTeam::EDM ? ProfileEDM3 : ProfileFB3)->SetVisibility(ESlateVisibility::Visible);
			break;
	}
}

void USMCharacterSelectorProfile::SetPlayerName(const FString& Name, const bool bIsOwner)
{
	PlayerName->SetText(FText::FromString(Name));
	PlayerName->SetColorAndOpacity(bIsOwner ? FLinearColor(FLinearColor(0.283149f, 0.996078f, 0.391573f, 1.0f)) : FLinearColor::White);
}

void USMCharacterSelectorProfile::SetPlayerReady(ASMPlayerState* Player, bool bIsReady)
{
	SetProfileImage(Player->GetTeam(), Player->GetCharacterType());
	bIsReady ? PlayAnimationForward(ReadyState) : PlayAnimationReverse(ReadyState);
}

void USMCharacterSelectorProfile::RemovePlayerInfo()
{
	ProfileEDM1->SetVisibility(ESlateVisibility::Hidden);
	ProfileEDM2->SetVisibility(ESlateVisibility::Hidden);
	ProfileEDM3->SetVisibility(ESlateVisibility::Hidden);

	ProfileFB1->SetVisibility(ESlateVisibility::Hidden);
	ProfileFB2->SetVisibility(ESlateVisibility::Hidden);
	ProfileFB3->SetVisibility(ESlateVisibility::Hidden);

	SetPlayerName("", false);
	SetProfileImage(ESMTeam::None, ESMCharacterType::None);
}
