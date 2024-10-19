// Copyright Studio Surround. All Rights Reserved.


#include "SMCharacterSelectorInformationWidget.h"

#include "CommonTextBlock.h"
#include "Components/Image.h"

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

	ResetInfo();
}

void USMCharacterSelectorInformationWidget::ResetInfo() const
{
	SelectedCharacterType->SetText(FText::FromString(""));

	for (int32 i = 0; i < 3; ++i)
	{
		if (SkillImages.IsValidIndex(i))
		{
			SkillImages[i]->SetBrushFromTexture(nullptr);
		}

		if (SkillNames.IsValidIndex(i))
		{
			SkillNames[i]->SetText(FText::FromString(""));
		}

		if (SkillDescriptions.IsValidIndex(i))
		{
			SkillDescriptions[i]->SetText(FText::FromString(""));
		}

		if (PlayerImages.IsValidIndex(i))
		{
			PlayerImages[i]->SetBrushFromTexture(nullptr);
		}

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

void USMCharacterSelectorInformationWidget::SetPlayerInfo(const TArray<FString>& InPlayerNames, const TArray<FString>& InPlayerCharacterTypes, const TArray<UTexture2D*>& InPlayerTextures)
{
	for (int32 i = 0; i < 3; ++i)
	{
		if (PlayerImages.IsValidIndex(i) && InPlayerTextures.IsValidIndex(i))
		{
			PlayerImages[i]->SetBrushFromTexture(InPlayerTextures[i]);
		}

		if (PlayerNames.IsValidIndex(i) && InPlayerNames.IsValidIndex(i))
		{
			PlayerNames[i]->SetText(FText::FromString(InPlayerNames[i]));
		}

		if (PlayerCharacterTypes.IsValidIndex(i) && InPlayerCharacterTypes.IsValidIndex(i))
		{
			PlayerCharacterTypes[i]->SetText(FText::FromString(InPlayerCharacterTypes[i]));
		}
	}
}

void USMCharacterSelectorInformationWidget::SetSkillInfo(const TArray<FString>& InSkillNames, const TArray<FString>& InSkillDescriptions, const TArray<UTexture2D*>& InSkillTextures)
{
	for (int32 i = 0; i < 3; ++i)
	{
		if (SkillImages.IsValidIndex(i) && InSkillTextures.IsValidIndex(i))
		{
			SkillImages[i]->SetBrushFromTexture(InSkillTextures[i]);
		}

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
