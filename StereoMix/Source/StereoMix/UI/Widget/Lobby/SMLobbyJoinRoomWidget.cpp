// Copyright Surround, Inc. All Rights Reserved.


#include "SMLobbyJoinRoomWidget.h"

#include "Kismet/GameplayStatics.h"
#include "StereoMixLog.h"
#include "UI/Widget/Frontend/SMFrontendWidget.h"
#include "UI/Widget/Popup/SMAlertPopup.h"

USMLobbyJoinRoomWidget::USMLobbyJoinRoomWidget()
{
}

void USMLobbyJoinRoomWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void USMLobbyJoinRoomWidget::NativeOnActivated()
{
	Super::NativeOnActivated();

	if (!AuthSubsystem->IsAuthenticated())
	{
		return;
	}

	if (LobbySubsystem)
	{
		LobbySubsystem->OnJoinRoomWithCodeResponse.AddDynamic(this, &USMLobbyJoinRoomWidget::OnJoinRoomWithCodeResponse);

		GetWorld()->GetTimerManager().SetTimerForNextTick([this] {
			RoomCode = GetParentFrontendWidget()->RequestRoomCode;
			UE_LOG(LogStereoMixUI, Log, TEXT("[%s] NativeOnActivated - InputRoomCode: %s"), *GetName(), *RoomCode)

			if (!LobbySubsystem->JoinRoomWithCode(RoomCode))
			{
				UiState = ELobbyProcessUiState::Failure;
				UE_LOG(LogStereoMixUI, Error, TEXT("[SMLobbyJoinRoomWidget] Failed to join room with code"));
				GetParentFrontendWidget()->ShowAlert(TEXT("방 참가에 실패했습니다."))->OnSubmit.BindWeakLambda(this, [&] { GetParentFrontendWidget()->RemoveElementWidget(this); });
				return;
			}

			UiState = ELobbyProcessUiState::Processing;
			SetProgressVisibility(ESlateVisibility::Visible);
			SetProgressText(FText::FromString(TEXT("방을 찾고 있습니다...")));
		});
	}
}

void USMLobbyJoinRoomWidget::NativeOnDeactivated()
{
	Super::NativeOnDeactivated();

	if (LobbySubsystem)
	{
		LobbySubsystem->OnJoinRoomWithCodeResponse.RemoveDynamic(this, &USMLobbyJoinRoomWidget::OnJoinRoomWithCodeResponse);
	}
}

void USMLobbyJoinRoomWidget::OnJoinRoomWithCodeResponse(const EJoinRoomWithCodeResult Result, const FString& ServerUrl)
{
	if (Result == EJoinRoomWithCodeResult::Success)
	{
		UiState = ELobbyProcessUiState::Success;
		ConnectToGameServer(ServerUrl);
		return;
	}

	switch (Result)
	{
		case EJoinRoomWithCodeResult::Cancelled:
			break;

		case EJoinRoomWithCodeResult::Unauthenticated:
			GetParentFrontendWidget()->ShowAlert(TEXT("인증 정보가 유효하지 않습니다."))->OnSubmit.BindWeakLambda(this, [&] { UGameplayStatics::OpenLevelBySoftObjectPtr(this, GetWorld()); });
			break;

		case EJoinRoomWithCodeResult::InvalidArgument:
			GetParentFrontendWidget()->ShowAlert(TEXT("입장 코드가 잘못되었습니다."))->OnSubmit.BindWeakLambda(this, [&] { GetParentFrontendWidget()->RemoveElementWidget(this); });
			break;

		case EJoinRoomWithCodeResult::RoomNotFound:
			GetParentFrontendWidget()->ShowAlert(TEXT("방을 찾지 못했습니다."))->OnSubmit.BindWeakLambda(this, [&] { GetParentFrontendWidget()->RemoveElementWidget(this); });
			break;

		case EJoinRoomWithCodeResult::RoomFull:
			GetParentFrontendWidget()->ShowAlert(TEXT("방이 꽉 찼습니다."))->OnSubmit.BindWeakLambda(this, [&] { GetParentFrontendWidget()->RemoveElementWidget(this); });
			break;

		case EJoinRoomWithCodeResult::ConnectionError:
			GetParentFrontendWidget()->ShowAlert(TEXT("서버와의 연결에 실패했습니다."))->OnSubmit.BindWeakLambda(this, [&] { UGameplayStatics::OpenLevelBySoftObjectPtr(this, GetWorld()); });
			break;

		case EJoinRoomWithCodeResult::UnknownError:
		case EJoinRoomWithCodeResult::InternalError:
		case EJoinRoomWithCodeResult::DeadlineExceeded: default:
			GetParentFrontendWidget()->ShowAlert(TEXT("방 참가에 실패했습니다."))->OnSubmit.BindWeakLambda(this, [&] { UGameplayStatics::OpenLevelBySoftObjectPtr(this, GetWorld()); });
			break;
	}

	UiState = ELobbyProcessUiState::Failure;
	SetProgressVisibility(ESlateVisibility::Collapsed);
}
