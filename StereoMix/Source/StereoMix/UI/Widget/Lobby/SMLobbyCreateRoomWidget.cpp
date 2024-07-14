// Copyright Surround, Inc. All Rights Reserved.


#include "SMLobbyCreateRoomWidget.h"

#include "Kismet/GameplayStatics.h"
#include "StereoMixLog.h"
#include "UI/Widget/Frontend/SMFrontendWidget.h"
#include "UI/Widget/Popup/SMAlertPopup.h"

USMLobbyCreateRoomWidget::USMLobbyCreateRoomWidget()
{
}

void USMLobbyCreateRoomWidget::NativeOnActivated()
{
	Super::NativeOnActivated();

	if (!AuthSubsystem->IsAuthenticated())
	{
		return;
	}

	if (LobbySubsystem)
	{
		LobbySubsystem->OnCreateRoomResponse.AddDynamic(this, &USMLobbyCreateRoomWidget::OnCreateRoomResponse);

		const FString& UserName = AuthSubsystem->GetUserAccount()->GetUserName();
		FString RoomName = FString::Printf(TEXT("%s의 방"), *UserName);

		FGrpcLobbyRoomConfig RoomConfig;
		RoomConfig.RoomName = RoomName;
		RoomConfig.Visibility = GetParentFrontendWidget()->CreateRoomOptions.RoomVisibility == ECreateRoomVisibilityOption::Public ? EGrpcLobbyRoomVisibility::ROOM_VISIBILITY_PUBLIC : EGrpcLobbyRoomVisibility::ROOM_VISIBILITY_PRIVATE;
		RoomConfig.Map = EGrpcLobbyGameMap::GAME_MAP_DEFAULT;
		RoomConfig.Mode = EGrpcLobbyGameMode::GAME_MODE_DEFAULT;
		if (!LobbySubsystem->CreateRoom(RoomName, RoomConfig))
		{
			UiState = ELobbyProcessUiState::Failure;
			UE_LOG(LogStereoMixUI, Error, TEXT("[SMLobbyCreateRoomWidget] Failed to create room"));

			USMAlertPopup* Alert = GetParentFrontendWidget()->ShowAlert(TEXT("방 생성에 실패했습니다."));
			Alert->OnSubmit.BindWeakLambda(this, [&](USMPopup*) { GetParentFrontendWidget()->RemoveElementWidget(this); });
			return;
		}

		UiState = ELobbyProcessUiState::Processing;
		SetProgressVisibility(ESlateVisibility::Visible);
		SetProgressText(FText::FromString(TEXT("새로운 방을 만들고 있습니다...")));
	}
}

void USMLobbyCreateRoomWidget::NativeOnDeactivated()
{
	Super::NativeOnDeactivated();

	if (LobbySubsystem)
	{
		LobbySubsystem->OnCreateRoomResponse.RemoveDynamic(this, &USMLobbyCreateRoomWidget::OnCreateRoomResponse);
	}
}

void USMLobbyCreateRoomWidget::OnCreateRoomResponse(const ECreateRoomResult Result, const FString& ServerUrl)
{
	if (Result == ECreateRoomResult::Success)
	{
		UiState = ELobbyProcessUiState::Success;
		ConnectToGameServer(ServerUrl);
		return;
	}

	switch (Result)
	{
		case ECreateRoomResult::Cancelled:
			break;

		case ECreateRoomResult::Unauthenticated:
		{
			GetParentFrontendWidget()->ShowAlert(TEXT("인증 정보가 유효하지 않습니다."))->OnSubmit.BindWeakLambda(this, [&](USMPopup*) { UGameplayStatics::OpenLevelBySoftObjectPtr(this, GetWorld()); });
			break;
		}

		case ECreateRoomResult::InvalidArgument:
			GetParentFrontendWidget()->ShowAlert(TEXT("방 생성 정보가 잘못되었습니다."))->OnSubmit.BindWeakLambda(this, [&](USMPopup*) { GetParentFrontendWidget()->RemoveElementWidget(this); });
			break;

		case ECreateRoomResult::ConnectionError:
			GetParentFrontendWidget()->ShowAlert(TEXT("서버와의 연결에 실패했습니다."))->OnSubmit.BindWeakLambda(this, [&](USMPopup*) { UGameplayStatics::OpenLevelBySoftObjectPtr(this, GetWorld()); });
			break;

		case ECreateRoomResult::UnknownError:
		case ECreateRoomResult::InternalError:
		case ECreateRoomResult::DeadlineExceeded: default:
			GetParentFrontendWidget()->ShowAlert(TEXT("방 생성에 실패했습니다."))->OnSubmit.BindWeakLambda(this, [&](USMPopup*) { GetParentFrontendWidget()->RemoveElementWidget(this); });
			break;
	}

	UiState = ELobbyProcessUiState::Failure;
	SetProgressVisibility(ESlateVisibility::Collapsed);
}
