// Copyright Surround, Inc. All Rights Reserved.


#include "SMLobbyHUD.h"

#include "Blueprint/UserWidget.h"
#include "Connection/SMClientConnectionSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "UI/ViewModel/SMViewModel_Lobby.h"
#include "UI/ViewModel/SMViewModel_RoomShortCodePopup.h"

ASMLobbyHUD::ASMLobbyHUD()
{
}

void ASMLobbyHUD::BeginPlay()
{
	Super::BeginPlay();

	if (MainWidget)
	{
		if (auto ViewModel = GetViewModelFromWidget<USMViewModel_Lobby>(MainWidget, TEXT("SMViewModel_Lobby")))
		{
			LobbyViewModel = ViewModel;
			LobbyViewModel->OnRequestJoinRoom.BindUObject(this, &ASMLobbyHUD::OnRequestJoinRoom);
		}
	}

	if (const auto ConnectionSubsystem = GetGameInstance()->GetSubsystem<USMClientConnectionSubsystem>())
	{
		ConnectionSubsystem->LobbyServiceStateChangedEvent.AddUniqueDynamic(this, &ASMLobbyHUD::OnLobbyServiceStateChanged);
		ConnectionSubsystem->CreateRoomEvent.AddUniqueDynamic(this, &ASMLobbyHUD::OnCreateRoom);
		ConnectionSubsystem->JoinRoomEvent.AddUniqueDynamic(this, &ASMLobbyHUD::OnJoinRoom);
		ConnectionSubsystem->ConnectLobbyService();

		if (ConnectionSubsystem->IsAuthenticated())
		{
			LobbyViewModel->SetStatusText(FText::GetEmpty());
		}
		else
		{
			UGameplayStatics::OpenLevel(GetWorld(), TEXT("L_Title"));
		}
	}
}

void ASMLobbyHUD::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (const auto ConnectionSubsystem = GetGameInstance()->GetSubsystem<USMClientConnectionSubsystem>())
	{
		ConnectionSubsystem->LobbyServiceStateChangedEvent.RemoveDynamic(this, &ASMLobbyHUD::OnLobbyServiceStateChanged);
		ConnectionSubsystem->CreateRoomEvent.RemoveDynamic(this, &ASMLobbyHUD::OnCreateRoom);
		ConnectionSubsystem->JoinRoomEvent.RemoveDynamic(this, &ASMLobbyHUD::OnJoinRoom);
	}
}

void ASMLobbyHUD::CreateRoomShortCodePopupWidget()
{
	checkfSlow(JoinRoomWithCodeWidgetClass, TEXT("JoinRoomWithCodeWidgetClass is not set."))
	JoinRoomWithCodeWidget = AddWidgetToViewport(JoinRoomWithCodeWidgetClass, 5);
	if (JoinRoomWithCodeWidget)
	{
		if (auto ViewModel = GetViewModelFromWidget<USMViewModel_RoomShortCodePopup>(JoinRoomWithCodeWidget, TEXT("SMViewModel_RoomShortCodePopup")))
		{
			RoomShortCodePopupViewModel = ViewModel;
			RoomShortCodePopupViewModel->OnSubmit.BindUObject(this, &ASMLobbyHUD::OnJoinRoomWithCodeSubmit);
			RoomShortCodePopupViewModel->OnCancel.BindUObject(this, &ASMLobbyHUD::OnJoinRoomWithCodeCancel);
		}
		LobbyViewModel->SetAllButtonEnabled(false);
	}
}

void ASMLobbyHUD::OnRequestJoinRoom()
{
	CreateRoomShortCodePopupWidget();
}

void ASMLobbyHUD::OnJoinRoomWithCodeSubmit()
{
	if (!RoomShortCodePopupViewModel)
	{
		return;
	}

	if (const auto ConnectionSubsystem = GetGameInstance()->GetSubsystem<USMClientConnectionSubsystem>())
	{
		ConnectionSubsystem->JoinRoomUsingShortCode(RoomShortCodePopupViewModel->GetRoomShortCodeText().ToUpper().ToString());
		JoinRoomWithCodeWidget->RemoveFromParent();
		LobbyViewModel->SetAllButtonEnabled(false);
	}
}

void ASMLobbyHUD::OnJoinRoomWithCodeCancel()
{
	JoinRoomWithCodeWidget->RemoveFromParent();
	LobbyViewModel->SetAllButtonEnabled(true);
}

void ASMLobbyHUD::OnLobbyServiceStateChanged(EGrpcServiceState ChangedState)
{
}

void ASMLobbyHUD::OnCreateRoom(EGrpcResultCode Result, EGrpcLobbyRoomDeploymentStatus DeployStatus, const FString& ConnectionUrl)
{
	if (!LobbyViewModel)
	{
		return;
	}
	const auto ConnectionSubsystem = GetGameInstance()->GetSubsystem<USMClientConnectionSubsystem>();
	if (!ConnectionSubsystem)
	{
		return;
	}

	// TODO: ViewModel로 옮기기
	if (Result != EGrpcResultCode::Ok)
	{
		LobbyViewModel->SetStatusText(FText::FromName(TEXT("방 생성에 실패했습니다.")));
		LobbyViewModel->SetAllButtonEnabled(true);
	}
	else
	{
		switch (DeployStatus)
		{
		case EGrpcLobbyRoomDeploymentStatus::ROOM_DEPLOYMENT_STATUS_INITIALIZING:
		case EGrpcLobbyRoomDeploymentStatus::ROOM_DEPLOYMENT_STATUS_SEEKING:
		case EGrpcLobbyRoomDeploymentStatus::ROOM_DEPLOYMENT_STATUS_SEEKED:
			LobbyViewModel->SetStatusText(FText::FromName(TEXT("방 생성을 요청 중 입니다...")));
			break;

		case EGrpcLobbyRoomDeploymentStatus::ROOM_DEPLOYMENT_STATUS_SCANNING:
			LobbyViewModel->SetStatusText(FText::FromName(TEXT("최적의 위치를 검색 중 입니다...")));
			break;

		case EGrpcLobbyRoomDeploymentStatus::ROOM_DEPLOYMENT_STATUS_DEPLOYING:
			LobbyViewModel->SetStatusText(FText::FromName(TEXT("방 생성 중 입니다...")));
			break;

		case EGrpcLobbyRoomDeploymentStatus::ROOM_DEPLOYMENT_STATUS_READY:
		// LobbyViewModel->SetStatus(FText::FromName(TEXT("방이 성공적으로 생성됐습니다!")));
			UGameplayStatics::OpenLevel(GetWorld(), FName(*ConnectionUrl), true, FString::Printf(TEXT("Nickname=%s"), *ConnectionSubsystem->GetAccount().UserName.ToString()));;
			break;

		case EGrpcLobbyRoomDeploymentStatus::ROOM_DEPLOYMENT_STATUS_TERMINATED:
		case EGrpcLobbyRoomDeploymentStatus::ROOM_DEPLOYMENT_STATUS_ERROR:
			LobbyViewModel->SetStatusText(FText::FromName(TEXT("방 생성에 실패했습니다.")));
			LobbyViewModel->SetAllButtonEnabled(true);
			break;

		case EGrpcLobbyRoomDeploymentStatus::ROOM_DEPLOYMENT_STATUS_UNSPECIFIED:
		default:
			LobbyViewModel->SetStatusText(FText::FromName(TEXT("")));
			break;
		}
	}
}

void ASMLobbyHUD::OnJoinRoom(EGrpcResultCode Result, const FString& ConnectionUrl)
{
	if (Result == EGrpcResultCode::Ok)
	{
		const auto ConnectionSubsystem = GetGameInstance()->GetSubsystem<USMClientConnectionSubsystem>();
		const FText& Nickname = ConnectionSubsystem->GetAccount().UserName;
		UGameplayStatics::OpenLevel(GetWorld(), FName(*ConnectionUrl), true, TEXT("Nickname=") + Nickname.ToString());
	}
	else
	{
		LobbyViewModel->SetStatusText(FText::FromName(TEXT("방 참가에 실패했습니다.")));
		LobbyViewModel->SetAllButtonEnabled(true);
	}
}
