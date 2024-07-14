// Copyright Surround, Inc. All Rights Reserved.


#include "SMLobbyWidget.h"

#include "SMCreateRoomPopup.h"
#include "SMLobbyCreateRoomWidget.h"
#include "SMLobbyJoinRoomWidget.h"
#include "SMLobbyQuickMatchWidget.h"
#include "SMLobbyRoomCodePopup.h"
#include "Animation/UMGSequencePlayer.h"
#include "Kismet/GameplayStatics.h"
#include "StereoMixLog.h"
#include "Backend/Client/SMClientLobbySubsystem.h"
#include "UI/Widget/Frontend/SMFrontendWidget.h"
#include "UI/Widget/Popup/SMAlertPopup.h"

USMLobbyWidget::USMLobbyWidget()
{
	bIsBackHandler = true;
	bAutoTransitionOnActivate = false;
}

void USMLobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CreateRoomButton->OnClicked().AddUObject(this, &USMLobbyWidget::OnCreateRoomButtonClicked);
	QuickMatchButton->OnClicked().AddUObject(this, &USMLobbyWidget::OnQuickMatchButtonClicked);
	JoinRoomButton->OnClicked().AddUObject(this, &USMLobbyWidget::OnJoinRoomButtonClicked);
}

void USMLobbyWidget::NativeOnActivated()
{
	Super::NativeOnActivated();

	LobbySubsystem = GetGameInstance()->GetSubsystem<USMClientLobbySubsystem>();
	if (LobbySubsystem)
	{
		LobbySubsystem->OnServiceStateChanged.AddDynamic(this, &USMLobbyWidget::OnLobbyServiceStateChanged);

		GetWorld()->GetTimerManager().SetTimerForNextTick([this] {
			if (LobbySubsystem->GetLobbyService()->GetServiceState() == EGrpcServiceState::Ready)
			{
				OnLobbyServiceStateChanged(EGrpcServiceState::Ready);
			}
			else
			{
				LobbySubsystem->Connect();
			}
		});
	}
}

void USMLobbyWidget::NativeOnDeactivated()
{
	Super::NativeOnDeactivated();

	if (LobbySubsystem)
	{
		LobbySubsystem->OnServiceStateChanged.RemoveDynamic(this, &USMLobbyWidget::OnLobbyServiceStateChanged);
	}
}

void USMLobbyWidget::NativeDestruct()
{
	Super::NativeDestruct();

	CreateRoomButton->OnClicked().Clear();
	QuickMatchButton->OnClicked().Clear();
	JoinRoomButton->OnClicked().Clear();
}

void USMLobbyWidget::OnCreateRoomButtonClicked()
{
	PopupWidget = GetParentFrontendWidget()->AddPopup(CreateRoomPopupClass);
	PopupWidget->OnSubmit.BindUObject(this, &USMLobbyWidget::OnSubmitCreateRoomOptions);
	PopupWidget->OnClose.BindUObject(this, &USMLobbyWidget::OnClosePopup);
}

void USMLobbyWidget::OnQuickMatchButtonClicked()
{
	GetParentFrontendWidget()->AddElementWidget(QuickMatchWidgetClass.Get());
}

void USMLobbyWidget::OnJoinRoomButtonClicked()
{
	PopupWidget = GetParentFrontendWidget()->AddPopup(RoomCodePopupClass);
	PopupWidget->OnSubmit.BindUObject(this, &USMLobbyWidget::OnSubmitRoomCode);
	PopupWidget->OnClose.BindUObject(this, &USMLobbyWidget::OnClosePopup);
}

void USMLobbyWidget::OnSubmitCreateRoomOptions(USMPopup* Popup)
{
	if (!PopupWidget)
	{
		UE_LOG(LogStereoMixUI, Error, TEXT("PopupWidget is nullptr"));
	}

	const USMCreateRoomPopup* CreateRoomPopup = CastChecked<USMCreateRoomPopup>(PopupWidget);
	GetParentFrontendWidget()->CreateRoomOptions = CreateRoomPopup->GetCreateRoomOptions();
	OnClosePopup();

	GetParentFrontendWidget()->AddElementWidget(CreateRoomWidgetClass.Get());
}

void USMLobbyWidget::OnSubmitRoomCode(USMPopup* Popup)
{
	if (!PopupWidget)
	{
		UE_LOG(LogStereoMixUI, Error, TEXT("PopupWidget is nullptr"));
	}

	const USMLobbyRoomCodePopup* RoomCodePopup = CastChecked<USMLobbyRoomCodePopup>(PopupWidget);
	GetParentFrontendWidget()->RequestRoomCode = RoomCodePopup->GetRoomCode().ToString();
	OnClosePopup();

	GetParentFrontendWidget()->AddElementWidget(JoinRoomWidgetClass.Get());
}

void USMLobbyWidget::OnClosePopup()
{
	if (PopupWidget)
	{
		PopupWidget->OnSubmit.Unbind();
		PopupWidget->OnClose.Unbind();
		PopupWidget = nullptr;
	}
}

void USMLobbyWidget::OnLobbyServiceStateChanged(const EGrpcServiceState ServiceState)
{
	if (ServiceState == EGrpcServiceState::Ready)
	{
		GetParentFrontendWidget()->ClearPopups();
		if (UUMGSequencePlayer* TransitionPlayer = PlayTransitionIn())
		{
			TransitionPlayer->OnSequenceFinishedPlaying().AddWeakLambda(this, [this](UUMGSequencePlayer&) { SetVisibility(ESlateVisibility::Visible); });
			SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		else
		{
			SetVisibility(ESlateVisibility::Visible);
		}
	}
	else if (ServiceState == EGrpcServiceState::TransientFailure)
	{
		USMAlertPopup* Alert = GetParentFrontendWidget()->ShowAlert(TEXT("서버와의 연결에 실패했습니다."));
		Alert->OnSubmit.BindWeakLambda(this, [&](USMPopup*) { UGameplayStatics::OpenLevelBySoftObjectPtr(this, GetWorld()); });
	}
}
