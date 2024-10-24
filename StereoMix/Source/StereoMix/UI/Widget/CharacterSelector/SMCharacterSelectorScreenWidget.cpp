﻿// Copyright Studio Surround. All Rights Reserved.


#include "SMCharacterSelectorScreenWidget.h"

#include "CommonTextBlock.h"
#include "EngineUtils.h"
#include "SMCharacterSelectorInformationWidget.h"
#include "SMCharacterSelectorTimerWidget.h"
#include "Animation/SkeletalMeshActor.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Button.h"
#include "Components/Overlay.h"
#include "Controllers/SMCharacterSelectPlayerController.h"
#include "Games/SMCountdownTimerComponent.h"
#include "Games/CharacterSelect/SMCharacterSelectPlayerState.h"
#include "Games/CharacterSelect/SMCharacterSelectState.h"


void USMCharacterSelectorScreenWidget::InitWidget(ASMCharacterSelectPlayerController* PlayerController, ASMCharacterSelectState* CharacterSelectState, ASMCharacterSelectPlayerState* PlayerState)
{
	if (ensure(PlayerController))
	{
		OwningPlayerController = PlayerController;
	}

	if (ensure(CharacterSelectState))
	{
		OwningCharacterSelectState = CharacterSelectState;
	}

	if (ensure(PlayerState))
	{
		OwningPlayerState = PlayerState;
	}

	TArray<UUserWidget*> FoundWidgets;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), FoundWidgets, UUserWidget::StaticClass(), false);

	for (UUserWidget* Widget : FoundWidgets)
	{
		if (CharacterSelectorInformationWidget == nullptr)
		{
			CharacterSelectorInformationWidget = Cast<USMCharacterSelectorInformationWidget>(Widget);
		}

		if (CharacterSelectorTimerWidget == nullptr)
		{
			CharacterSelectorTimerWidget = Cast<USMCharacterSelectorTimerWidget>(Widget);
		}

		if (CharacterSelectorInformationWidget.IsValid() && CharacterSelectorTimerWidget.IsValid())
		{
			break;
		}
	}

	OriginalSelectButtonStyle = PickElectricGuitar->GetStyle();
	OriginalSelectButtonStyle.Normal = PickElectricGuitar->GetStyle().Normal;

	OriginalSelectedButtonStyle = PickElectricGuitar->GetStyle();
	OriginalSelectedButtonStyle.Normal = PickElectricGuitar->GetStyle().Disabled;

	PickElectricGuitar->OnClicked.AddDynamic(this, &ThisClass::OnPickElectricGuitar);
	PickPiano->OnClicked.AddDynamic(this, &ThisClass::OnPickPiano);
	PickBass->OnClicked.AddDynamic(this, &ThisClass::OnPickBass);

	SelectButton->OnClicked.AddDynamic(this, &ThisClass::OnSelectButtonClicked);

	OwningCharacterSelectState->OnPlayerJoined.AddDynamic(this, &ThisClass::OnPlayerJoin);
	OwningCharacterSelectState->OnPlayerLeft.AddDynamic(this, &ThisClass::OnPlayerLeft);
	OwningCharacterSelectState->OnPlayerCharacterChanged.AddDynamic(this, &ThisClass::OnPlayerCharacterChanged);

	OwningPlayerState->OnCharacterChangeResponse.AddDynamic(this, &ThisClass::OnCharacterChangeResponse);

	OwningCharacterSelectState->GetCountdownTimer()->OnCountdownTick.AddDynamic(this, &ThisClass::OnCountdownTick);
	OwningCharacterSelectState->OnCurrentStateChanged.AddDynamic(this, &ThisClass::OnCurrentStateChanged);

	TArray AvailableCharacterTypes = { ESMCharacterType::ElectricGuitar, ESMCharacterType::Piano, ESMCharacterType::Bass };

	ChangeFocusedCharacter(AvailableCharacterTypes[FMath::RandRange(0, AvailableCharacterTypes.Num() - 1)]);
}

void USMCharacterSelectorScreenWidget::OnCountdownTick()
{
	if (CharacterSelectorTimerWidget.IsValid())
	{
		const int32 RemainingTime = OwningCharacterSelectState->GetCountdownTimer()->GetRemainingTime();
		CharacterSelectorTimerWidget->SetRemainTime(RemainingTime);
	}
}

void USMCharacterSelectorScreenWidget::OnCurrentStateChanged(ECharacterSelectionStateType NewCharacterSelectionState)
{
	if (NewCharacterSelectionState == ECharacterSelectionStateType::End)
	{
		CharacterSelectBox->SetVisibility(ESlateVisibility::Hidden);

		if (FocusedCharacterType != ESMCharacterType::None)
		{
			ChangeFocusedCharacter(OwningPlayerState->GetCharacterType());
		}
	}
}

void USMCharacterSelectorScreenWidget::OnPickElectricGuitar()
{
	if (FocusedCharacterType == ESMCharacterType::ElectricGuitar)
	{
		return;
	}

	GetOwningPlayerState()->ChangeCharacterType(ESMCharacterType::None);

	ChangeFocusedCharacter(ESMCharacterType::ElectricGuitar);
}

void USMCharacterSelectorScreenWidget::OnPickPiano()
{
	if (FocusedCharacterType == ESMCharacterType::Piano)
	{
		return;
	}

	GetOwningPlayerState()->ChangeCharacterType(ESMCharacterType::None);

	ChangeFocusedCharacter(ESMCharacterType::Piano);
}

void USMCharacterSelectorScreenWidget::OnPickBass()
{
	if (FocusedCharacterType == ESMCharacterType::Bass)
	{
		return;
	}

	GetOwningPlayerState()->ChangeCharacterType(ESMCharacterType::None);

	ChangeFocusedCharacter(ESMCharacterType::Bass);
}

void USMCharacterSelectorScreenWidget::OnSelectButtonClicked()
{
	SelectButton->SetIsEnabled(false);
	if (IsFocusedCharacterSelectable(false))
	{
		bIsNeverSelected = false;
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

	if (const int32 PlayerIndex = OwningCharacterSelectState->PlayerArray.IndexOfByKey(Player); PlayerIndex != INDEX_NONE)
	{
		if (CharacterSelectorInformationWidget.IsValid())
		{
			CharacterSelectorInformationWidget->SetPlayerReady(PlayerIndex, NewCharacter != ESMCharacterType::None);
		}
	}

	if (Player == OwningPlayerState && NewCharacter != ESMCharacterType::None && FocusedCharacterType != NewCharacter)
	{
		ChangeFocusedCharacter(NewCharacter);
	}
	else
	{
		UpdatePlayerList();
		UpdateSelectButton();
	}
}

void USMCharacterSelectorScreenWidget::OnCharacterChangeResponse(bool bSuccess, ESMCharacterType NewCharacterType)
{
	if (NewCharacterType == ESMCharacterType::None)
	{
		return;
	}

	if (!bSuccess || !IsFocusedCharacterSelectable(true))
	{
		UE_LOG(LogTemp, Warning, TEXT("Character change failed"));
		GetOwningPlayerState()->ChangeCharacterType(ESMCharacterType::None);
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Character changed successfully"));
}

void USMCharacterSelectorScreenWidget::ChangeFocusedCharacter(const ESMCharacterType CharacterType)
{
	if (FocusedCharacterType == CharacterType)
	{
		return;
	}

	FocusedCharacterType = CharacterType;

	PickElectricGuitar->SetStyle(CharacterType == ESMCharacterType::ElectricGuitar ? OriginalSelectedButtonStyle : OriginalSelectButtonStyle);
	PickPiano->SetStyle(CharacterType == ESMCharacterType::Piano ? OriginalSelectedButtonStyle : OriginalSelectButtonStyle);
	PickBass->SetStyle(CharacterType == ESMCharacterType::Bass ? OriginalSelectedButtonStyle : OriginalSelectButtonStyle);

	UpdatePlayerList();
	UpdateSelectButton();
	ShowPreviewCharacter(FocusedCharacterType);
	if (CharacterSelectorInformationWidget.IsValid())
	{
		CharacterSelectorInformationWidget->SetSkillInfo(FocusedCharacterType);
	}
}

void USMCharacterSelectorScreenWidget::ShowPreviewCharacter(const ESMCharacterType CharacterType)
{
	FName TargetName = TEXT("ABP_None_FB");
	const ESMTeam LocalTeam = GetOwningPlayerState()->GetTeam();
	switch (CharacterType)
	{
		case ESMCharacterType::None:
			break;
		case ESMCharacterType::ElectricGuitar:
			TargetName = LocalTeam == ESMTeam::EDM ? TEXT("ABP_ElectricGuitar_EDM") : TEXT("ABP_ElectricGuitar_FB");
			break;
		case ESMCharacterType::Piano:
			TargetName = LocalTeam == ESMTeam::EDM ? TEXT("ABP_Piano_EDM") : TEXT("ABP_Piano_FB");
			break;
		case ESMCharacterType::Bass:
			TargetName = LocalTeam == ESMTeam::EDM ? TEXT("ABP_Bass_EDM") : TEXT("ABP_Bass_FB");
			break;
	}

	if (CharacterMesh)
	{
		CharacterMesh->SetActorHiddenInGame(true);
	}

	if (const UWorld* World = GetWorld())
	{
		for (ASkeletalMeshActor* Actor : TActorRange<ASkeletalMeshActor>(World))
		{
			if (Actor->GetActorLabel() == TargetName)
			{
				CharacterMesh = Actor;
				CharacterMesh->SetActorHiddenInGame(false);
				break;
			}
		}
	}
}

void USMCharacterSelectorScreenWidget::UpdatePlayerList() const
{
	const ASMCharacterSelectState* CharacterSelectState = GetOwningCharacterSelectState();
	if (!CharacterSelectState || !CharacterSelectorInformationWidget.IsValid())
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
	SelectButtonText->SetText(FText::FromString(bIsNeverSelected ? TEXT("확인") : TEXT("변경")));
	SelectButton->SetIsEnabled(IsFocusedCharacterSelectable(false));
}

bool USMCharacterSelectorScreenWidget::IsFocusedCharacterSelectable(bool bExcludeOwner) const
{
	bool bSelectButtonEnable = true;

	const ESMTeam LocalPlayerTeam = GetOwningPlayerState()->GetTeam();
	for (TArray<TObjectPtr<APlayerState>> Players = OwningCharacterSelectState->PlayerArray; TObjectPtr<APlayerState> PlayerState : Players)
	{
		if (bExcludeOwner && PlayerState == OwningPlayerState)
		{
			continue;
		}

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
