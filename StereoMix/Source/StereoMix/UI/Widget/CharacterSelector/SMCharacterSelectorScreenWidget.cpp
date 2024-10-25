// Copyright Studio Surround. All Rights Reserved.


#include "SMCharacterSelectorScreenWidget.h"

#include "CommonTextBlock.h"
#include "EngineUtils.h"
#include "NiagaraActor.h"
#include "NiagaraComponent.h"
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

	if (OwningPlayerState->GetTeam() == ESMTeam::EDM)
	{
		FBElectricGuitar->SetVisibility(ESlateVisibility::Hidden);
		FBPiano->SetVisibility(ESlateVisibility::Hidden);
		FBBass->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		EDMElectricGuitar->SetVisibility(ESlateVisibility::Hidden);
		EDMPiano->SetVisibility(ESlateVisibility::Hidden);
		EDMBass->SetVisibility(ESlateVisibility::Hidden);
	}

	OriginalSelectButtonStyle = PickElectricGuitar->GetStyle();
	OriginalSelectButtonStyle.Normal = PickElectricGuitar->GetStyle().Normal;

	OriginalSelectedButtonStyle = PickElectricGuitar->GetStyle();
	OriginalSelectedButtonStyle.Normal = PickElectricGuitar->GetStyle().Disabled;

	OriginalDisabledButtonStyle = PickElectricGuitar->GetStyle();
	OriginalDisabledButtonStyle.Normal = PickElectricGuitar->GetStyle().Hovered;
	OriginalDisabledButtonStyle.Normal.SetImageSize(FVector2D(162.0, 162.0));

	PickElectricGuitar->OnClicked.AddDynamic(this, &ThisClass::OnPickElectricGuitar);
	PickPiano->OnClicked.AddDynamic(this, &ThisClass::OnPickPiano);
	PickBass->OnClicked.AddDynamic(this, &ThisClass::OnPickBass);

	SelectButton->OnClicked.AddDynamic(this, &ThisClass::OnSelectButtonClicked);

	OwningCharacterSelectState->OnPlayerJoined.AddDynamic(this, &ThisClass::OnPlayerJoin);
	OwningCharacterSelectState->OnPlayerLeft.AddDynamic(this, &ThisClass::OnPlayerLeft);
	OwningCharacterSelectState->OnPlayerCharacterChanged.AddDynamic(this, &ThisClass::OnPlayerCharacterChanged);
	OwningCharacterSelectState->OnPlayerCharacterFocusChanged.AddDynamic(this, &ThisClass::ChangeFocusedCharacter);

	OwningCharacterSelectState->GetCountdownTimer()->OnCountdownTick.AddDynamic(this, &ThisClass::OnCountdownTick);
	OwningCharacterSelectState->OnCurrentStateChanged.AddDynamic(this, &ThisClass::OnCurrentStateChanged);

	UpdatePlayerList();
	
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
		bSelectEnabled = false;
		CharacterSelectBox->SetVisibility(ESlateVisibility::Hidden);

		if (FocusedCharacterType != ESMCharacterType::None)
		{
			ChangeFocusedCharacter(OwningPlayerState->GetCharacterType());
		}
	}
}

void USMCharacterSelectorScreenWidget::OnPickElectricGuitar()
{
	ChangeFocusedCharacter(ESMCharacterType::ElectricGuitar);
}

void USMCharacterSelectorScreenWidget::OnPickPiano()
{
	ChangeFocusedCharacter(ESMCharacterType::Piano);
}

void USMCharacterSelectorScreenWidget::OnPickBass()
{
	ChangeFocusedCharacter(ESMCharacterType::Bass);
}

void USMCharacterSelectorScreenWidget::OnSelectButtonClicked()
{
	if (!bSelectEnabled)
	{
		return;
	}

	// 캐릭터 변경 딜레이
	bSelectEnabled = false;
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [WeakPtr = MakeWeakObjectPtr(this)] { WeakPtr.IsValid() ? WeakPtr->bSelectEnabled = true : false; }, 0.5f, false);

	SelectButton->SetIsEnabled(false);
	if (IsFocusedCharacterSelectable(true))
	{
		bIsNeverSelected = false;
		SetPlayerReady(GetOwningPlayerState(), FocusedCharacterType, true, true);
		GetOwningPlayerState()->ChangeCharacterType(FocusedCharacterType);
	}
}

void USMCharacterSelectorScreenWidget::OnPlayerJoin(ASMPlayerState* JoinedPlayer)
{
	if (JoinedPlayer->GetTeam() == GetOwningPlayerState()->GetTeam())
	{
		CurrentTeamPlayers.AddUnique(JoinedPlayer);
		UpdatePlayerList();
		UpdateSelectButton();
	}
}

void USMCharacterSelectorScreenWidget::OnPlayerLeft(ASMPlayerState* LeftPlayer)
{
	SetPlayerReady(LeftPlayer, ESMCharacterType::None, false);
	CurrentTeamPlayers.Remove(LeftPlayer);
	UpdatePlayerList();
	UpdateSelectButton();
}

void USMCharacterSelectorScreenWidget::OnPlayerCharacterChanged(ASMPlayerState* Player, ESMCharacterType NewCharacter)
{
	if (Player->GetTeam() != GetOwningPlayerState()->GetTeam())
	{
		return;
	}

	const bool IsOwningPlayer = (Player == GetOwningPlayerState());
	const bool IsValidCharacter = (NewCharacter != ESMCharacterType::None);
	const bool IsDifferentCharacter = (FocusedCharacterType != NewCharacter);

	UpdatePlayerList();
	UpdateSelectButton();

	if (IsOwningPlayer && IsValidCharacter && IsDifferentCharacter)
	{
		FocusedCharacterType = NewCharacter;
		GetOwningPlayerState()->SetPredictFocusCharacterType(FocusedCharacterType);
		GetOwningPlayerState()->ChangeFocusCharacterType(FocusedCharacterType);
		ShowPreviewCharacter(FocusedCharacterType);
		if (CharacterSelectorInformationWidget.IsValid())
		{
			CharacterSelectorInformationWidget->SetSkillInfo(FocusedCharacterType);
		}
	}
}

void USMCharacterSelectorScreenWidget::ChangeFocusedCharacter(ASMPlayerState* Player, ESMCharacterType NewCharacter)
{
	UpdatePlayerList();
	UpdateSelectButton();
}

TArray<TObjectPtr<ASMPlayerState>> USMCharacterSelectorScreenWidget::GetCurrentTeamPlayers() const
{
	TArray<TObjectPtr<ASMPlayerState>> PlayerArray = CurrentTeamPlayers;
	PlayerArray.Sort([](const APlayerState& A, const APlayerState& B) {
		return A.GetPlayerId() < B.GetPlayerId();
	});

	return PlayerArray;
}

void USMCharacterSelectorScreenWidget::ChangeFocusedCharacter(const ESMCharacterType CharacterType)
{
	if (!bSelectEnabled || FocusedCharacterType == CharacterType)
	{
		return;
	}

	GetOwningPlayerState()->ChangeCharacterType(ESMCharacterType::None);

	FocusedCharacterType = CharacterType;
	GetOwningPlayerState()->SetPredictFocusCharacterType(CharacterType);
	GetOwningPlayerState()->ChangeFocusCharacterType(CharacterType);

	UpdatePlayerList();
	UpdateSelectButton();
	SetPlayerReady(GetOwningPlayerState(), CharacterType, false, true);

	ShowPreviewCharacter(FocusedCharacterType);
	if (CharacterSelectorInformationWidget.IsValid())
	{
		CharacterSelectorInformationWidget->SetSkillInfo(FocusedCharacterType);
	}
}

void USMCharacterSelectorScreenWidget::ShowPreviewCharacter(const ESMCharacterType CharacterType)
{
	FName TargetTag = TEXT("ABP_None_FB");
	FName SelectFXTag = TEXT("EDMCharacterSelectFX");
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
		ANiagaraActor* SelectFX = nullptr;
		for (ANiagaraActor* Actor : TActorRange<ANiagaraActor>(World))
		{
			if (Actor->ActorHasTag(SelectFXTag))
			{
				SelectFX = Actor;
				break;
			}
		}

		if (SelectFX == nullptr)
		{
			return;
		}
		
		for (ASkeletalMeshActor* Actor : TActorRange<ASkeletalMeshActor>(World))
		{
			if (Actor->ActorHasTag(TargetTag))
			{
				CharacterMesh = Actor;
				CharacterMesh->SetActorHiddenInGame(false);
				if (const USkeletalMeshComponent* MeshComponent = CharacterMesh->GetSkeletalMeshComponent())
				{
					const wchar_t* const AttachBoneName = TEXT("b-Spine-0");
					const FVector BoneLocation = MeshComponent->GetBoneLocation(AttachBoneName, static_cast<EBoneSpaces::Type>(RTS_World));
					SelectFX->SetActorLocation(BoneLocation);
					SelectFX->GetNiagaraComponent()->ResetSystem();
					SelectFX->GetNiagaraComponent()->Activate();
				}
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

	TArray<ASMCharacterSelectPlayerState*> PlayerArray;

	for (TObjectPtr<APlayerState> PlayerState : GetCurrentTeamPlayers())
	{
		if (ASMCharacterSelectPlayerState* TargetPlayerState = Cast<ASMCharacterSelectPlayerState>(PlayerState))
		{
			PlayerArray.Add(TargetPlayerState);
		}
	}

	if (const int32 PlayerIndex = GetCurrentTeamPlayers().IndexOfByKey(GetOwningPlayerState()); PlayerIndex != INDEX_NONE)
	{
		CharacterSelectorInformationWidget->SetPlayerInfo(PlayerArray, PlayerIndex);
	}
}

void USMCharacterSelectorScreenWidget::UpdateSelectButton()
{
	const bool bIsSelectable = IsFocusedCharacterSelectable(true);
	const FButtonStyle NewButtonStyle = bIsSelectable ? OriginalSelectedButtonStyle : OriginalDisabledButtonStyle;

	PickElectricGuitar->SetStyle(FocusedCharacterType == ESMCharacterType::ElectricGuitar ? NewButtonStyle : OriginalSelectButtonStyle);
	PickPiano->SetStyle(FocusedCharacterType == ESMCharacterType::Piano ? NewButtonStyle : OriginalSelectButtonStyle);
	PickBass->SetStyle(FocusedCharacterType == ESMCharacterType::Bass ? NewButtonStyle : OriginalSelectButtonStyle);

	for (auto& Profile : { EDMElectricGuitar, EDMPiano, EDMBass, FBElectricGuitar, FBPiano, FBBass })
	{
		constexpr float EnabledScalar = 0.0f;
		Profile->GetDynamicMaterial()->SetScalarParameterValue(TEXT("State"), EnabledScalar);
	}

	for (TObjectPtr<ASMPlayerState> Player : GetCurrentTeamPlayers())
	{
		if (const ASMCharacterSelectPlayerState* TargetPlayerState = Cast<ASMCharacterSelectPlayerState>(Player))
		{
			if (TargetPlayerState == GetOwningPlayerState())
			{
				continue;
			}

			constexpr float SoftDisabledScalar = 1.0f;
			constexpr float DisabledScalar = 2.0f;
			const float SelectedScalar = TargetPlayerState->GetCharacterType() == FocusedCharacterType ? SoftDisabledScalar : DisabledScalar;

			switch (TargetPlayerState->GetCharacterType())
			{
				case ESMCharacterType::ElectricGuitar:
					(TargetPlayerState->GetTeam() == ESMTeam::EDM ? EDMElectricGuitar : FBElectricGuitar)->GetDynamicMaterial()->SetScalarParameterValue(TEXT("State"), SelectedScalar);
					break;
				case ESMCharacterType::Piano:
					(TargetPlayerState->GetTeam() == ESMTeam::EDM ? EDMPiano : FBPiano)->GetDynamicMaterial()->SetScalarParameterValue(TEXT("State"), SelectedScalar);
					break;
				case ESMCharacterType::Bass:
					(TargetPlayerState->GetTeam() == ESMTeam::EDM ? EDMBass : FBBass)->GetDynamicMaterial()->SetScalarParameterValue(TEXT("State"), SelectedScalar);
					break;
				default:
					break;
			}
		}
	}

	SelectButtonText->SetText(FText::FromString(bIsNeverSelected ? TEXT("확인") : TEXT("변경")));
	SelectButton->SetIsEnabled(IsFocusedCharacterSelectable(false));

	WarningText->SetVisibility(ESlateVisibility::Hidden);
	if (!bIsSelectable)
	{
		PlayAnimationForward(OnDuplicatedWarning);
	}
}

bool USMCharacterSelectorScreenWidget::IsCharacterSelectable(ESMCharacterType CharacterType, bool bExcludeOwner) const
{
	bool bSelectButtonEnable = true;

	for (TObjectPtr<APlayerState> PlayerState : GetCurrentTeamPlayers())
	{
		if (bExcludeOwner && PlayerState == GetOwningPlayerState())
		{
			continue;
		}

		if (const ASMCharacterSelectPlayerState* TargetPlayerState = Cast<ASMCharacterSelectPlayerState>(PlayerState))
		{
			if (TargetPlayerState->GetCharacterType() == CharacterType)
			{
				bSelectButtonEnable = false;
				break;
			}
		}
	}

	return bSelectButtonEnable;
}

bool USMCharacterSelectorScreenWidget::IsFocusedCharacterSelectable(bool bExcludeOwner) const
{
	return IsCharacterSelectable(FocusedCharacterType, bExcludeOwner);
}

void USMCharacterSelectorScreenWidget::SetPlayerReady(ASMPlayerState* Player, const ESMCharacterType CharacterType, const bool bIsReady, const bool bIsPredicated) const
{
	if (CharacterSelectorInformationWidget.IsValid())
	{
		const TArray<ASMPlayerState*> PlayerArray = GetCurrentTeamPlayers();
		if (const int32 PlayerIndex = PlayerArray.IndexOfByKey(Player); PlayerIndex != INDEX_NONE)
		{
			CharacterSelectorInformationWidget->SetPlayerReady(PlayerIndex, CharacterType, bIsReady, bIsPredicated);
		}
	}
}
