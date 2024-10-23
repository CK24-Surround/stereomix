// Copyright Studio Surround. All Rights Reserved.


#include "SMCharacterSelectorInformationWidget.h"

#include "CommonTextBlock.h"
#include "Components/Image.h"
#include "Data/SMCharacterType.h"

void USMCharacterSelectorInformationWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SkillImages.Add(SkillImage1);
	SkillImages.Add(SkillImage2);
	SkillImages.Add(SkillImage3);

	SkillNames.Add(SkillName1);
	SkillNames.Add(SkillName2);
	SkillNames.Add(SkillName3);

	SkillDescriptions.Add(SkillDescription1);
	SkillDescriptions.Add(SkillDescription2);
	SkillDescriptions.Add(SkillDescription3);

	PlayerImages.Add(PlayerImage1);
	PlayerImages.Add(PlayerImage2);
	PlayerImages.Add(PlayerImage3);

	PlayerNames.Add(PlayerName1);
	PlayerNames.Add(PlayerName2);
	PlayerNames.Add(PlayerName3);

	PlayerCharacterTypes.Add(PlayerCharacterType1);
	PlayerCharacterTypes.Add(PlayerCharacterType2);
	PlayerCharacterTypes.Add(PlayerCharacterType3);

	PlayerReadyAnimations.Add(Player1Ready);
	PlayerReadyAnimations.Add(Player2Ready);
	PlayerReadyAnimations.Add(Player3Ready);

	ResetInfo();
}

void USMCharacterSelectorInformationWidget::ResetInfo() const
{
	SelectedCharacterType->SetText(FText::FromString(""));

	for (int32 i = 0; i < 3; ++i)
	{
		// if (SkillImages.IsValidIndex(i))
		// {
		// 	SkillImages[i]->SetBrushFromTexture(nullptr);
		// }

		if (SkillNames.IsValidIndex(i))
		{
			SkillNames[i]->SetText(FText::FromString(""));
		}

		if (SkillDescriptions.IsValidIndex(i))
		{
			SkillDescriptions[i]->SetText(FText::FromString(""));
		}

		// if (PlayerImages.IsValidIndex(i))
		// {
		// 	PlayerImages[i]->SetBrushFromTexture(nullptr);
		// }

		if (PlayerNames.IsValidIndex(i))
		{
			PlayerNames[i]->SetText(FText::FromString(""));
		}

		if (PlayerCharacterTypes.IsValidIndex(i))
		{
			PlayerCharacterTypes[i]->SetText(FText::FromString(""));
		}
	}
}

void USMCharacterSelectorInformationWidget::ResetPlayerInfo() const
{
	for (int32 i = 0; i < 3; ++i)
	{
		// if (PlayerImages.IsValidIndex(i))
		// {
		// 	PlayerImages[i]->SetBrushFromTexture(nullptr);
		// }

		if (PlayerNames.IsValidIndex(i))
		{
			PlayerNames[i]->SetText(FText::FromString(""));
		}

		if (PlayerCharacterTypes.IsValidIndex(i))
		{
			PlayerCharacterTypes[i]->SetText(FText::FromString(""));
		}
	}
}

void USMCharacterSelectorInformationWidget::SetPlayerInfo(const TArray<FString>& InPlayerNames, const TArray<ESMCharacterType>& InPlayerCharacterTypes)
{
	for (int32 i = 0; i < 3; ++i)
	{
		// if (PlayerImages.IsValidIndex(i) && InPlayerTextures.IsValidIndex(i))
		// {
		// 	PlayerImages[i]->SetBrushFromTexture(InPlayerTextures[i]);
		// }

		if (InPlayerNames.IsValidIndex(i) && InPlayerCharacterTypes.IsValidIndex(i))
		{
			FString CharacterTypeString;
			switch (InPlayerCharacterTypes[i]) {
				case ESMCharacterType::None:
					CharacterTypeString = TEXT("선택중...");
					break;
				case ESMCharacterType::ElectricGuitar:
					CharacterTypeString = TEXT("일렉기타");
					break;
				case ESMCharacterType::Piano:
					CharacterTypeString = TEXT("피아노");
					break;
				case ESMCharacterType::Bass:
					CharacterTypeString = TEXT("베이스");
					break;
			}
			PlayerNames[i]->SetText(FText::FromString(InPlayerNames[i]));
			PlayerCharacterTypes[i]->SetText(FText::FromString(CharacterTypeString));
		}
	}
}

void USMCharacterSelectorInformationWidget::SetSkillInfo(const ESMCharacterType InPlayerCharacterTypes)
{
	TArray<UTexture2D*> InSkillTextures;
	TArray<FString> InSkillNames;
	TArray<FString> InSkillDescriptions;

	switch (InPlayerCharacterTypes) {
		case ESMCharacterType::None:
			break;
		case ESMCharacterType::ElectricGuitar:
			SelectedCharacterType->SetText(FText::FromString(TEXT("일렉기타")));
			InSkillNames.Add(TEXT("속주"));
			InSkillDescriptions.Add(TEXT("전방에 작은 탄환들을 빠른속도로 발사합니다"));
			InSkillNames.Add(TEXT("찌릿!"));
			InSkillDescriptions.Add(TEXT("마비탄을 발사해 적을 느려지게 합니다"));
			InSkillNames.Add(TEXT("파워 스크래치"));
			InSkillDescriptions.Add(TEXT("적과 함께 지정한 방향으로 빠르게 대쉬합니다"));
			break;
		case ESMCharacterType::Piano:
			SelectedCharacterType->SetText(FText::FromString(TEXT("피아노")));
			InSkillNames.Add(TEXT("차징샷"));
			InSkillDescriptions.Add(TEXT("에너지를 모은 화살을 충전해 발사합니다"));
			InSkillNames.Add(TEXT("저리가!"));
			InSkillDescriptions.Add(TEXT("선택한 지점에 충격화살을 발사해 적을 밀어냅니다"));
			InSkillNames.Add(TEXT("피날레"));
			InSkillDescriptions.Add(TEXT("하늘로 뛰어오른뒤 지정한 위치로 적을 발사합니다"));
			break;
		case ESMCharacterType::Bass:
			SelectedCharacterType->SetText(FText::FromString(TEXT("베이스")));
			InSkillNames.Add(TEXT("헤비 스윙"));
			InSkillDescriptions.Add(TEXT("베이스를 휘둘러 전방의 넓은 범위를 공격합니다"));
			InSkillNames.Add(TEXT("내가 간다!"));
			InSkillDescriptions.Add(TEXT("무적상태가 되어 돌진합니다. 부딪힌 적은 기절합니다"));
			InSkillNames.Add(TEXT("스테이지 다이브"));
			InSkillDescriptions.Add(TEXT("하늘로 뛰어올라 적을 바닥에 내리꽂습니다"));
			break;
	}
	
	for (int32 i = 0; i < 3; ++i)
	{
		// if (SkillImages.IsValidIndex(i) && InSkillTextures.IsValidIndex(i))
		// {
		// 	SkillImages[i]->SetBrushFromTexture(InSkillTextures[i]);
		// }

		if (SkillNames.IsValidIndex(i) && InSkillNames.IsValidIndex(i))
		{
			SkillNames[i]->SetText(FText::FromString(InSkillNames[i]));
		}

		if (SkillDescriptions.IsValidIndex(i) && InSkillDescriptions.IsValidIndex(i))
		{
			SkillDescriptions[i]->SetText(FText::FromString(InSkillDescriptions[i]));
		}
	}
}

void USMCharacterSelectorInformationWidget::SetPlayerReady(const int32 PlayerIndex, const bool bIsReady)
{
	if (PlayerReadyAnimations.IsValidIndex(PlayerIndex))
	{
		if (bIsReady)
		{
			PlayAnimationForward(PlayerReadyAnimations[PlayerIndex]);
		}
		else
		{
			PlayAnimationReverse(PlayerReadyAnimations[PlayerIndex]);
		}
	}
}
