// Copyright Studio Surround. All Rights Reserved.


#include "SMCharacterSelectorScreenWidget.h"

#include "CommonTextBlock.h"
#include "SMCharacterSelectorInformationWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Button.h"
#include "Games/CharacterSelect/SMCharacterSelectPlayerState.h"
#include "Games/CharacterSelect/SMCharacterSelectState.h"


void USMCharacterSelectorScreenWidget::InitWidget(ASMCharacterSelectState* CharacterSelectState, ASMCharacterSelectPlayerState* PlayerState)
{
	if (ensure(CharacterSelectState))
	{
		OwningCharacterSelectState = CharacterSelectState;
	}

	if (ensure(PlayerState))
	{
		OwningPlayerState = PlayerState;
	}

	TArray<UUserWidget*> FoundWidgets;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), FoundWidgets, USMCharacterSelectorInformationWidget::StaticClass(), false);
	for (UUserWidget* Widget : FoundWidgets)
	{
		if (!Widget)
		{
			continue;
		}

		CharacterSelectorInformationWidget = Cast<USMCharacterSelectorInformationWidget>(Widget);
		break;
	}

	PickElectricGuitar->OnClicked.AddDynamic(this, &ThisClass::OnPickElectricGuitar);
	PickPiano->OnClicked.AddDynamic(this, &ThisClass::OnPickPiano);
	PickBass->OnClicked.AddDynamic(this, &ThisClass::OnPickBass);

	SelectButton->OnClicked.AddDynamic(this, &ThisClass::OnSelectButtonClicked);

	OwningCharacterSelectState->OnPlayerJoined.AddDynamic(this, &ThisClass::OnPlayerJoin);
	OwningCharacterSelectState->OnPlayerLeft.AddDynamic(this, &ThisClass::OnPlayerLeft);
	OwningCharacterSelectState->OnPlayerCharacterChanged.AddDynamic(this, &ThisClass::OnPlayerCharacterChanged);

	OwningPlayerState->OnCharacterChangeResponse.AddDynamic(this, &ThisClass::OnCharacterChangeResponse);

	UpdatePlayerList();
}

void USMCharacterSelectorScreenWidget::OnPickElectricGuitar()
{
	GetOwningPlayerState()->ChangeCharacterType(ESMCharacterType::None);

	FocusedCharacterType = ESMCharacterType::ElectricGuitar;
	CharacterSelectorInformationWidget->SetSkillInfo(ESMCharacterType::ElectricGuitar);

	UpdateSelectButton();
}

void USMCharacterSelectorScreenWidget::OnPickPiano()
{
	GetOwningPlayerState()->ChangeCharacterType(ESMCharacterType::None);

	FocusedCharacterType = ESMCharacterType::Piano;
	CharacterSelectorInformationWidget->SetSkillInfo(ESMCharacterType::Piano);

	UpdateSelectButton();
}

void USMCharacterSelectorScreenWidget::OnPickBass()
{
	GetOwningPlayerState()->ChangeCharacterType(ESMCharacterType::None);

	FocusedCharacterType = ESMCharacterType::Bass;
	CharacterSelectorInformationWidget->SetSkillInfo(ESMCharacterType::Bass);

	UpdateSelectButton();
}

void USMCharacterSelectorScreenWidget::OnSelectButtonClicked()
{
	if (IsFocusedCharacterSelectable())
	{
		SelectButton->SetIsEnabled(false);
		GetOwningPlayerState()->ChangeCharacterType(FocusedCharacterType);
	}
}

void USMCharacterSelectorScreenWidget::OnPlayerJoin(ASMPlayerState* JoinedPlayer)
{
	UpdatePlayerList();
	UpdateSelectButton();
}

void USMCharacterSelectorScreenWidget::OnPlayerLeft(ASMPlayerState* LeftPlayer)
{
	UpdatePlayerList();
	UpdateSelectButton();
}

void USMCharacterSelectorScreenWidget::OnPlayerCharacterChanged(ASMPlayerState* Player, ESMCharacterType NewCharacter)
{
	if (Player->GetTeam() != OwningPlayerState->GetTeam())
	{
		return;
	}

	UpdatePlayerList();
	UpdateSelectButton();
}

void USMCharacterSelectorScreenWidget::OnCharacterChangeResponse(bool bSuccess, ESMCharacterType NewCharacterType)
{
	if (!bSuccess)
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Character changed successfully"));
	// SelectButton->SetIsEnabled(false);
}

void USMCharacterSelectorScreenWidget::UpdatePlayerList() const
{
	const ASMCharacterSelectState* CharacterSelectState = GetOwningCharacterSelectState();
	if (!CharacterSelectState || !CharacterSelectorInformationWidget)
	{
		return;
	}

	CharacterSelectorInformationWidget->ResetPlayerInfo();

	TArray<FString> PlayerNames;
	TArray<ESMCharacterType> PlayerCharacterTypes;

	const ESMTeam LocalPlayerTeam = GetOwningPlayerState()->GetTeam();
	for (TArray<TObjectPtr<APlayerState>> Players = CharacterSelectState->PlayerArray; TObjectPtr<APlayerState> PlayerState : Players)
	{
		if (const ASMCharacterSelectPlayerState* TargetPlayerState = Cast<ASMCharacterSelectPlayerState>(PlayerState))
		{
			if (TargetPlayerState->GetTeam() == LocalPlayerTeam)
			{
				PlayerNames.Add(TargetPlayerState->GetPlayerName());
				PlayerCharacterTypes.Add(TargetPlayerState->GetCharacterType());
			}
		}
	}

	CharacterSelectorInformationWidget->SetPlayerInfo(PlayerNames, PlayerCharacterTypes);
}

void USMCharacterSelectorScreenWidget::UpdateSelectButton() const
{
	SelectButton->SetIsEnabled(false);
	SelectButtonText->SetText(FText::FromString(FocusedCharacterType == ESMCharacterType::None ? TEXT("확인") : TEXT("변경")));
	SelectButton->SetIsEnabled(IsFocusedCharacterSelectable());
}

bool USMCharacterSelectorScreenWidget::IsFocusedCharacterSelectable() const
{
	bool bSelectButtonEnable = true;

	const ESMTeam LocalPlayerTeam = GetOwningPlayerState()->GetTeam();
	for (TArray<TObjectPtr<APlayerState>> Players = OwningCharacterSelectState->PlayerArray; TObjectPtr<APlayerState> PlayerState : Players)
	{
		if (const ASMCharacterSelectPlayerState* TargetPlayerState = Cast<ASMCharacterSelectPlayerState>(PlayerState))
		{
			if (TargetPlayerState->GetTeam() == LocalPlayerTeam && TargetPlayerState->GetCharacterType() == FocusedCharacterType)
			{
				bSelectButtonEnable = false;
				break;
			}
		}
	}

	return bSelectButtonEnable;
}
