// Copyright Surround, Inc. All Rights Reserved.


#include "SMLobbyWidget.h"

#include "UI/Widget/Frontend/SMFrontendWidget.h"
#include "StereoMixLog.h"
#include "Animation/UMGSequencePlayer.h"
#include "Kismet/GameplayStatics.h"

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

	LobbyPopupStack->ClearWidgets();
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
	GetParentFrontendWidget()->AddElementWidget(CreateRoomWidgetClass);
}

void USMLobbyWidget::OnQuickMatchButtonClicked()
{
	GetParentFrontendWidget()->AddElementWidget(QuickMatchWidgetClass);
}

void USMLobbyWidget::OnJoinRoomButtonClicked()
{
	UE_LOG(LogStereoMixUI, Verbose, TEXT("[%s] OnJoinRoomButtonClicked"), *GetName())
	// if (RoomCodePopup.IsValid())
	// {
	// 	return;
	// }

	RoomCodePopup = LobbyPopupStack->AddWidget<USMLobbyRoomCodePopup>(RoomCodePopupClass);
	RoomCodePopup->OnSubmit.BindUObject(this, &USMLobbyWidget::OnSubmitRoomCode);
	RoomCodePopup->OnClose.BindUObject(this, &USMLobbyWidget::OnCloseRoomCodePopup);

	SetIsFocusable(false);
}

void USMLobbyWidget::OnSubmitRoomCode()
{
	UE_LOG(LogStereoMixUI, Verbose, TEXT("[%s] OnSubmitRoomCode"), *GetName())

	// TODO: 임시로 RoomCode를 RequestRoomCode에 저장하고 JoinRoomWidget을 띄움. 추후 룸 코드를 직접 넘길 수 있도록 변경해야 함.
	GetParentFrontendWidget()->RequestRoomCode = RoomCodePopup->GetRoomCode().ToString();
	GetParentFrontendWidget()->AddElementWidget(JoinRoomWidgetClass);
	SetIsFocusable(true);

	if (RoomCodePopup.IsValid())
	{
		RoomCodePopup->OnSubmit.Unbind();
		RoomCodePopup->OnClose.Unbind();
	}
}

void USMLobbyWidget::OnCloseRoomCodePopup()
{
	SetIsFocusable(true);

	if (RoomCodePopup.IsValid())
	{
		RoomCodePopup->OnSubmit.Unbind();
		RoomCodePopup->OnClose.Unbind();
		RoomCodePopup = nullptr;
	}
}

void USMLobbyWidget::OnLobbyServiceStateChanged(const EGrpcServiceState ServiceState)
{
	if (ServiceState == EGrpcServiceState::Ready)
	{
		if (UUMGSequencePlayer* TransitionPlayer = PlayTransitionIn())
		{
			TransitionPlayer->OnSequenceFinishedPlaying().AddWeakLambda(this,
				[this](UUMGSequencePlayer&) {
					SetVisibility(ESlateVisibility::Visible);
				});
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
		Alert->OnSubmit.BindWeakLambda(this,
			[&] {
				UGameplayStatics::OpenLevelBySoftObjectPtr(this, GetWorld());
			});
	}
}