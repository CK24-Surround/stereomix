// Copyright Studio Surround. All Rights Reserved.


#include "SMCharacterSelectorProfile.h"

#include "CommonTextBlock.h"
#include "Components/Image.h"
#include "Games/SMPlayerState.h"

class ASMPlayerState;

void USMCharacterSelectorProfile::SetProfileImage(ESMTeam Team, ESMCharacterType CharacterType, ESMCharacterType FocusCharacterType)
{
	constexpr float EnabledScalar = 0.0f;
	constexpr float DisabledScalar = 2.0f;

	// 모든 프로필 비활성화
	for (auto& Profile : { ESMCharacterType::ElectricGuitar, ESMCharacterType::Piano, ESMCharacterType::Bass })
	{
		SetVisibilityByCharacterType(Team, Profile, ESlateVisibility::Hidden, DisabledScalar);
	}

	if (FocusCharacterType == ESMCharacterType::None)
	{
		return;
	}

	SetCharacterType(CharacterType);

	// 선택된 상태에서 포커스를 변경한 경우
	if (CharacterType == ESMCharacterType::None || CharacterType != FocusCharacterType)
	{
		SetPlayerReady(false);
		SetVisibilityByCharacterType(Team, FocusCharacterType, ESlateVisibility::Visible, DisabledScalar);
		return;
	}

	// 선택된 캐릭터 프로필 활성화
	SetPlayerReady(CharacterType != ESMCharacterType::None);
	SetVisibilityByCharacterType(Team, CharacterType, ESlateVisibility::Visible, EnabledScalar);
}

void USMCharacterSelectorProfile::SetPlayerName(const FString& Name, const bool bIsOwner)
{
	PlayerName->SetText(FText::FromString(Name));
	PlayerName->SetColorAndOpacity(bIsOwner ? FLinearColor(FLinearColor(0.283149f, 0.996078f, 0.391573f, 1.0f)) : FLinearColor::White);
}

void USMCharacterSelectorProfile::SetCharacterType(const ESMCharacterType CharacterType)
{
	const wchar_t* CharacterTypeText = TEXT("");
	switch (CharacterType)
	{
		case ESMCharacterType::None:
			CharacterTypeText = TEXT("선택중...");
			break;
		case ESMCharacterType::ElectricGuitar:
			CharacterTypeText = TEXT("일렉기타");
			break;
		case ESMCharacterType::Piano:
			CharacterTypeText = TEXT("피아노");
			break;
		case ESMCharacterType::Bass:
			CharacterTypeText = TEXT("베이스");
			break;
	}
	PlayerCharacterType->SetText(FText::FromString(CharacterTypeText));
}

void USMCharacterSelectorProfile::SetPlayerReady(bool bIsReady)
{
	if (!bIsReady)
	{
		PlayerReadyState->SetVisibility(ESlateVisibility::Hidden);
		return;
	}

	// if (IsAnimationPlayingForward(ReadyState))
	// {
	// 	return;
	// }

	PlayerReadyState->SetVisibility(ESlateVisibility::Visible);
	// PlayAnimationForward(ReadyState);
}

void USMCharacterSelectorProfile::SetProfileImageScalar(float InScalar)
{
	for (auto& Profile : { ProfileEDM1, ProfileEDM2, ProfileEDM3, ProfileFB1, ProfileFB2, ProfileFB3 })
	{
		Profile->GetDynamicMaterial()->SetScalarParameterValue(TEXT("State"), InScalar);
	}
}

void USMCharacterSelectorProfile::SetVisibilityByCharacterType(ESMTeam Team, ESMCharacterType InCharacterType, ESlateVisibility InVisibility, float InScalar)
{
	SetProfileImageScalar(InScalar);
	switch (InCharacterType)
	{
		case ESMCharacterType::None:
			break;
		case ESMCharacterType::ElectricGuitar:
			(Team == ESMTeam::EDM ? ProfileEDM1 : ProfileFB1)->SetVisibility(InVisibility);
			break;
		case ESMCharacterType::Piano:
			(Team == ESMTeam::EDM ? ProfileEDM2 : ProfileFB2)->SetVisibility(InVisibility);
			break;
		case ESMCharacterType::Bass:
			(Team == ESMTeam::EDM ? ProfileEDM3 : ProfileFB3)->SetVisibility(InVisibility);
			break;
	}
}

void USMCharacterSelectorProfile::RemovePlayerInfo()
{
	for (auto& Profile : { ProfileEDM1, ProfileEDM2, ProfileEDM3, ProfileFB1, ProfileFB2, ProfileFB3 })
	{
		Profile->SetVisibility(ESlateVisibility::Hidden);
	}

	SetPlayerReady(false);
	SetPlayerName("", false);
	PlayerCharacterType->SetText(FText::FromString(TEXT("")));
}
