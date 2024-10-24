// Copyright Studio Surround. All Rights Reserved.


#include "SMCharacterSelectorProfile.h"

#include "CommonTextBlock.h"
#include "Components/Image.h"
#include "Games/SMPlayerState.h"
#include "Games/CharacterSelect/SMCharacterSelectPlayerState.h"

class ASMPlayerState;

void USMCharacterSelectorProfile::SetProfileImage(ESMTeam Team, ESMCharacterType CharacterType, ESMCharacterType FocusCharacterType)
{
	constexpr float EnabledScalar = 0.0f;
	constexpr float DisabledScalar = 2.0f;

	auto SetVisibilityByCharacterType = [this, Team](ESMCharacterType InCharacterType, ESlateVisibility InVisibility, float InScalar = EnabledScalar) {
		switch (InCharacterType)
		{
			case ESMCharacterType::None:
				break;
			case ESMCharacterType::ElectricGuitar:
				(Team == ESMTeam::EDM ? ProfileEDM1 : ProfileFB1)->SetVisibility(InVisibility);
				(Team == ESMTeam::EDM ? ProfileEDM1 : ProfileFB1)->GetDynamicMaterial()->SetScalarParameterValue(TEXT("State"), InScalar);
				break;
			case ESMCharacterType::Piano:
				(Team == ESMTeam::EDM ? ProfileEDM2 : ProfileFB2)->SetVisibility(InVisibility);
				(Team == ESMTeam::EDM ? ProfileEDM2 : ProfileFB2)->GetDynamicMaterial()->SetScalarParameterValue(TEXT("State"), InScalar);
				break;
			case ESMCharacterType::Bass:
				(Team == ESMTeam::EDM ? ProfileEDM3 : ProfileFB3)->SetVisibility(InVisibility);
				(Team == ESMTeam::EDM ? ProfileEDM3 : ProfileFB3)->GetDynamicMaterial()->SetScalarParameterValue(TEXT("State"), InScalar);
				break;
		}
	};

	// 모든 프로필 비활성화
	for (auto& Profile : { ESMCharacterType::ElectricGuitar, ESMCharacterType::Piano, ESMCharacterType::Bass })
	{
		SetVisibilityByCharacterType(Profile, ESlateVisibility::Hidden, DisabledScalar);
	}

	if (FocusCharacterType == ESMCharacterType::None)
	{
		return;
	}

	// 선택된 상태에서 포커스를 변경한 경우
	if (FocusCharacterType != CharacterType)
	{
		SetVisibilityByCharacterType(FocusCharacterType, ESlateVisibility::Visible, DisabledScalar);
		return;
	}

	// 선택된 캐릭터 프로필 활성화
	SetVisibilityByCharacterType(CharacterType, ESlateVisibility::Visible);
	switch (CharacterType)
	{
		case ESMCharacterType::None:
			PlayerCharacterType->SetText(FText::FromString(TEXT("선택중...")));
			break;
		case ESMCharacterType::ElectricGuitar:
			PlayerCharacterType->SetText(FText::FromString(TEXT("일렉기타")));
			break;
		case ESMCharacterType::Piano:
			PlayerCharacterType->SetText(FText::FromString(TEXT("피아노")));
			break;
		case ESMCharacterType::Bass:
			PlayerCharacterType->SetText(FText::FromString(TEXT("베이스")));
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
	if (const ASMCharacterSelectPlayerState* PlayerState = Cast<ASMCharacterSelectPlayerState>(Player))
	{
		SetProfileImage(PlayerState->GetTeam(), PlayerState->GetCharacterType(), PlayerState->GetFocusCharacterType());
		bIsReady ? PlayAnimationForward(ReadyState) : PlayAnimationReverse(ReadyState);
	}
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
	SetProfileImage(ESMTeam::None, ESMCharacterType::None, ESMCharacterType::None);
}
