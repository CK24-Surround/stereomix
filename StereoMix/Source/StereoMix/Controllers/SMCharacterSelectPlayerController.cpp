// Copyright Surround, Inc. All Rights Reserved.


#include "SMCharacterSelectPlayerController.h"

#include "EngineUtils.h"
#include "Games/CharacterSelect/SMCharacterSelectPlayerState.h"
#include "Utilities/SMLog.h"

ASMCharacterSelectPlayerController::ASMCharacterSelectPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Type::Default;
}

void ASMCharacterSelectPlayerController::BeginPlay()
{
	Super::BeginPlay();
	// SetInputMode(FInputModeUIOnly());
}

void ASMCharacterSelectPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	LoadingScreenWidget = CreateWidget<USMLoadingScreenWidget>(this, LoadingScreenWidgetClass);
	LoadingScreenWidget->SetLoadingText(FText::FromString(TEXT("다른 플레이어들을 기다리는 중 입니다...")));
	LoadingScreenWidget->AddToViewport(10);

	if (GetWorld()->GetGameState())
	{
		NET_LOG(this, Verbose, TEXT("GameState is already set"))
		InitPlayer();
	}
	else
	{
		NET_LOG(this, Verbose, TEXT("GameState is not set yet. Register GameStateSetEvent."))
		GetWorld()->GameStateSetEvent.AddLambda([this](AGameStateBase*)
		{
			NET_LOG(this, Verbose, TEXT("GameStateSetEvent"))
			InitPlayer();
		});
	}
}

void ASMCharacterSelectPlayerController::InitPlayer()
{
	CharacterSelectState = GetWorld()->GetGameStateChecked<ASMCharacterSelectState>();
	CharacterSelectState->OnCurrentStateChanged.AddDynamic(this, &ASMCharacterSelectPlayerController::OnCurrentCharacterSelectStateChanged);
	NET_LOG(this, Verbose, TEXT("Register CurrentStateChanged event"))
	CharacterSelectPlayerState = Cast<ASMCharacterSelectPlayerState>(PlayerState);
	CharacterSelectPlayerState->OnCharacterChangeResponse.AddDynamic(this, &ASMCharacterSelectPlayerController::OnCharacterChangeResponse);

	ESMTeam Team = CharacterSelectPlayerState->GetTeam();
	if (Team == ESMTeam::None)
	{
		Team = ESMTeam::EDM;
	}
	for (TActorIterator<AActor> It(GetWorld(), ASMPreviewCharacter::StaticClass()); It; ++It)
	{
		if (ASMPreviewCharacter* TargetCharacter = Cast<ASMPreviewCharacter>(*It); TargetCharacter && TargetCharacter->GetTeam() == Team)
		{
			NET_LOG(this, Verbose, TEXT("PreviewCharacter: %s - %s"), *UEnum::GetValueAsString(TargetCharacter->GetTeam()), *UEnum::GetValueAsString(TargetCharacter->GetCharacterType()))
			PreviewCharacters.Add(TargetCharacter->GetCharacterType(), TargetCharacter);
		}
	}

	CharacterSelectPlayerState->SetCurrentState(ECharacterSelectPlayerStateType::Unready);
}

void ASMCharacterSelectPlayerController::OnCurrentCharacterSelectStateChanged(ECharacterSelectionStateType NewCharacterSelectionState)
{
	NET_LOG(this, Verbose, TEXT("Current character selection state: %s"), *UEnum::GetValueAsString(NewCharacterSelectionState));
	if (NewCharacterSelectionState == ECharacterSelectionStateType::Select)
	{
		LoadingScreenWidget->HideLoadingScreen();
		CharacterSelectWidget = CreateWidget<USMCharacterSelectWidget>(this, CharacterSelectWidgetClass);
		CharacterSelectWidget->InitWidget(GetCharacterSelectState(), GetCharacterSelectPlayerState());
		CharacterSelectWidget->AddToViewport();
	}
}

void ASMCharacterSelectPlayerController::OnCharacterChangeResponse(bool bSuccess, ESMCharacterType NewCharacterType)
{
	if (bSuccess)
	{
		if (const TObjectPtr<ASMPreviewCharacter>* PreviewCharacter = PreviewCharacters.Find(NewCharacterType))
		{
			PreviewCharacter->Get()->PlaySelectAnimation();
		}
	}
}
