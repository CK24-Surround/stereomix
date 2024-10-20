// Copyright Studio Surround. All Rights Reserved.


#include "SMCharacterSelectorScreenWidget.h"

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

	OwningCharacterSelectState->OnPlayerJoined.AddDynamic(this, &ThisClass::OnPlayerJoin);
	OwningCharacterSelectState->OnPlayerLeft.AddDynamic(this, &ThisClass::OnPlayerLeft);
	OwningCharacterSelectState->OnPlayerCharacterChanged.AddDynamic(this, &ThisClass::OnPlayerCharacterChanged);

	OwningPlayerState->OnCharacterChangeResponse.AddDynamic(this, &ThisClass::OnCharacterChangeResponse);

	UpdatePlayerList();
}

void USMCharacterSelectorScreenWidget::OnPickElectricGuitar()
{
	GetOwningPlayerState()->ChangeCharacterType(ESMCharacterType::None);
	CharacterSelectorInformationWidget->SetSkillInfo(ESMCharacterType::ElectricGuitar);
}

void USMCharacterSelectorScreenWidget::OnPickPiano()
{
	GetOwningPlayerState()->ChangeCharacterType(ESMCharacterType::None);
	CharacterSelectorInformationWidget->SetSkillInfo(ESMCharacterType::Piano);
}

void USMCharacterSelectorScreenWidget::OnPickBass()
{
	GetOwningPlayerState()->ChangeCharacterType(ESMCharacterType::None);
	CharacterSelectorInformationWidget->SetSkillInfo(ESMCharacterType::Bass);
}

void USMCharacterSelectorScreenWidget::OnPlayerJoin(ASMPlayerState* JoinedPlayer)
{
	UpdatePlayerList();
}

void USMCharacterSelectorScreenWidget::OnPlayerLeft(ASMPlayerState* LeftPlayer)
{
	UpdatePlayerList();
}

void USMCharacterSelectorScreenWidget::OnPlayerCharacterChanged(ASMPlayerState* Player, ESMCharacterType NewCharacter)
{
	if (Player->GetTeam() != OwningPlayerState->GetTeam())
	{
		return;
	}

	UpdatePlayerList();
}

void USMCharacterSelectorScreenWidget::OnCharacterChangeResponse(bool bSuccess, ESMCharacterType NewCharacterType)
{
	if (!bSuccess)
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Character changed successfully"));
	UpdatePlayerList();
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
