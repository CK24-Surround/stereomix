// Copyright Surround, Inc. All Rights Reserved.


#include "SMCharacterSelectButton.h"

#include "SMCharacterSelectWidget.h"

void USMCharacterSelectButton::InitWidget(USMCharacterSelectWidget* Widget)
{
	ParentCharacterSelectWidget = Widget;

	Team = Widget->GetOwningPlayerState()->GetTeam();

	if (Team == ESMTeam::EDM)
	{
		CharacterData = FSMCharacterSelectButtonData(EdmCharacterData);
	}
	else if (Team == ESMTeam::FutureBass)
	{
		CharacterData = FSMCharacterSelectButtonData(FutureBassCharacterData);
	}
	else
	{
		// 테스트용
		CharacterData = FSMCharacterSelectButtonData(FutureBassCharacterData);
	}

	ButtonImage->SetBrushFromTexture(DefaultButtonTexture);
	InteractableSection->SetVisibility(ESlateVisibility::Hidden);

	SetIsEnabled(!bNotSupportedCharacter);

	if (GetIsEnabled())
	{
		CharacterImage->SetBrushFromTexture(CharacterData.DefaultTexture);
	}
	else
	{
		CharacterImage->SetBrushFromTexture(CharacterData.DisabledTexture);
	}

	Widget->GetOwningPlayerState()->OnCharacterChangeResponse.AddDynamic(this, &USMCharacterSelectButton::OnCharacterChangeResponse);
}

void USMCharacterSelectButton::NativePreConstruct()
{
	Super::NativePreConstruct();
}

void USMCharacterSelectButton::NativeConstruct()
{
	Super::NativeConstruct();
}

void USMCharacterSelectButton::NativeOnHovered()
{
	Super::NativeOnHovered();
	QueuePlayAnimationForward(HoverAnim);
	CharacterImage->SetBrushFromTexture(CharacterData.FocusedTexture);
	ButtonImage->SetBrushFromTexture(FocusedButtonTexture);
	InteractableSection->SetVisibility(ESlateVisibility::Visible);
}

void USMCharacterSelectButton::NativeOnUnhovered()
{
	Super::NativeOnUnhovered();
	QueuePlayAnimationReverse(HoverAnim);
	CharacterImage->SetBrushFromTexture(CharacterData.DefaultTexture);
	ButtonImage->SetBrushFromTexture(DefaultButtonTexture);
	InteractableSection->SetVisibility(ESlateVisibility::Hidden);
}

void USMCharacterSelectButton::NativeOnEnabled()
{
	Super::NativeOnEnabled();
	CharacterImage->SetBrushFromTexture(CharacterData.DefaultTexture);
}

void USMCharacterSelectButton::NativeOnDisabled()
{
	Super::NativeOnDisabled();
	if (bNotSupportedCharacter)
	{
		CharacterImage->SetBrushFromTexture(CharacterData.DisabledTexture);
	}
	else
	{
		CharacterImage->SetBrushFromTexture(CharacterData.FocusedTexture);
	}
	InteractableSection->SetVisibility(ESlateVisibility::Hidden);
}

void USMCharacterSelectButton::NativeOnClicked()
{
	Super::NativeOnClicked();
	GetParentWidget()->GetOwningPlayerState()->ChangeCharacterType(CharacterType);
	SetIsEnabled(false);
}

void USMCharacterSelectButton::OnCharacterChangeResponse(bool bSuccess, ESMCharacterType NewCharacterType)
{
	if (!bSuccess)
	{
		return;
	}

	if (NewCharacterType == CharacterType)
	{
		SetIsEnabled(false);
	}
	else if (!bNotSupportedCharacter)
	{
		SetIsEnabled(true);
	}
}
