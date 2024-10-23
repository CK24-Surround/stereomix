// Copyright Studio Surround. All Rights Reserved.


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
#include "Utilities/SMLog.h"


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

	for (TObjectPtr<APlayerState> Player : GetOwningCharacterSelectState()->PlayerArray)
	{
		if (ASMPlayerState* TargetPlayerState = Cast<ASMPlayerState>(Player))
		{
			if (TargetPlayerState->GetTeam() == OwningPlayerState->GetTeam())
			{
				CurrentTeamPlayers.AddUnique(TargetPlayerState);
			}
		}
	}

	EDMProfiles->SetVisibility(OwningPlayerState->GetTeam() == ESMTeam::EDM ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
	FBProfiles->SetVisibility(OwningPlayerState->GetTeam() == ESMTeam::FutureBass ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);

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
	if (JoinedPlayer->GetTeam() == OwningPlayerState->GetTeam())
	{
		CurrentTeamPlayers.AddUnique(JoinedPlayer);
		UpdatePlayerList();
		UpdateSelectButton();
	}
}

void USMCharacterSelectorScreenWidget::OnPlayerLeft(ASMPlayerState* LeftPlayer)
{
	SetPlayerReady(LeftPlayer, false);
	CurrentTeamPlayers.Remove(LeftPlayer);
	UpdatePlayerList();
	UpdateSelectButton();
}

void USMCharacterSelectorScreenWidget::OnPlayerCharacterChanged(ASMPlayerState* Player, ESMCharacterType NewCharacter)
{
	if (Player->GetTeam() != OwningPlayerState->GetTeam())
	{
		return;
	}

	SetPlayerReady(Player, NewCharacter != ESMCharacterType::None);

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

TArray<TObjectPtr<ASMPlayerState>> USMCharacterSelectorScreenWidget::GetCurrentTeamPlayers() const
{
	TArray<TObjectPtr<ASMPlayerState>> PlayerArray = CurrentTeamPlayers;
	PlayerArray.Sort([](const APlayerState& A, const APlayerState& B) {
		return A.GetUniqueID() < B.GetUniqueID();
	});

	return PlayerArray;
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
	FName TargetTag = TEXT("ABP_None_FB");
	const ESMTeam LocalTeam = GetOwningPlayerState()->GetTeam();
	switch (CharacterType)
	{
		case ESMCharacterType::None:
			break;
		case ESMCharacterType::ElectricGuitar:
			TargetTag = LocalTeam == ESMTeam::EDM ? TEXT("ElectricGuitar_EDM") : TEXT("ElectricGuitar_FB");
			break;
		case ESMCharacterType::Piano:
			TargetTag = LocalTeam == ESMTeam::EDM ? TEXT("Piano_EDM") : TEXT("Piano_FB");
			break;
		case ESMCharacterType::Bass:
			TargetTag = LocalTeam == ESMTeam::EDM ? TEXT("Bass_EDM") : TEXT("Bass_FB");
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
			if (Actor->ActorHasTag(TargetTag))
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

	for (TObjectPtr<APlayerState> PlayerState : GetCurrentTeamPlayers())
	{
		if (const ASMCharacterSelectPlayerState* TargetPlayerState = Cast<ASMCharacterSelectPlayerState>(PlayerState))
		{
			PlayerNames.Add(TargetPlayerState->GetPlayerName());
			PlayerCharacterTypes.Add(TargetPlayerState->GetCharacterType());
		}
	}

	CharacterSelectorInformationWidget->SetPlayerInfo(PlayerNames, PlayerCharacterTypes);
}

void USMCharacterSelectorScreenWidget::UpdateSelectButton() const
{
	constexpr FLinearColor DisabledColor = FLinearColor(0.045f, 0.045f, 0.045f, 1.f);
	constexpr FLinearColor EnabledColor = FLinearColor(1.f, 1.f, 1.f, 1.f);

	EDMElectricGuitar->SetBrushTintColor(EnabledColor);
	EDMPiano->SetBrushTintColor(EnabledColor);
	EDMBass->SetBrushTintColor(EnabledColor);

	FBElectricGuitar->SetBrushTintColor(EnabledColor);
	FBPiano->SetBrushTintColor(EnabledColor);
	FBBass->SetBrushTintColor(EnabledColor);

	for (TObjectPtr<ASMPlayerState> Player : GetCurrentTeamPlayers())
	{
		if (const ASMCharacterSelectPlayerState* TargetPlayerState = Cast<ASMCharacterSelectPlayerState>(Player))
		{
			if (TargetPlayerState == OwningPlayerState || TargetPlayerState->GetCharacterType() == ESMCharacterType::None)
			{
				continue;
			}

			switch (TargetPlayerState->GetCharacterType())
			{
				case ESMCharacterType::ElectricGuitar:
					(TargetPlayerState->GetTeam() == ESMTeam::EDM ? EDMElectricGuitar : FBElectricGuitar)->SetBrushTintColor(DisabledColor);
					break;
				case ESMCharacterType::Piano:
					(TargetPlayerState->GetTeam() == ESMTeam::EDM ? EDMPiano : FBPiano)->SetBrushTintColor(DisabledColor);
					break;
				case ESMCharacterType::Bass:
					(TargetPlayerState->GetTeam() == ESMTeam::EDM ? EDMBass : FBBass)->SetBrushTintColor(DisabledColor);
					break;
				default:
					break;
			}
		}
	}

	SelectButtonText->SetText(FText::FromString(bIsNeverSelected ? TEXT("확인") : TEXT("변경")));
	SelectButton->SetIsEnabled(IsFocusedCharacterSelectable(false));
}

bool USMCharacterSelectorScreenWidget::IsFocusedCharacterSelectable(bool bExcludeOwner) const
{
	bool bSelectButtonEnable = true;

	for (TObjectPtr<APlayerState> PlayerState : GetCurrentTeamPlayers())
	{
		if (bExcludeOwner && PlayerState == OwningPlayerState)
		{
			continue;
		}

		if (const ASMCharacterSelectPlayerState* TargetPlayerState = Cast<ASMCharacterSelectPlayerState>(PlayerState))
		{
			if (TargetPlayerState->GetCharacterType() == FocusedCharacterType)
			{
				bSelectButtonEnable = false;
				break;
			}
		}
	}

	return bSelectButtonEnable;
}

void USMCharacterSelectorScreenWidget::SetPlayerReady(ASMPlayerState* Player, const bool bIsReady) const
{
	if (CharacterSelectorInformationWidget.IsValid())
	{
		if (const int32 PlayerIndex = GetCurrentTeamPlayers().IndexOfByKey(Player); PlayerIndex != INDEX_NONE)
		{
			CharacterSelectorInformationWidget->SetPlayerReady(PlayerIndex, bIsReady);
		}
	}
}
