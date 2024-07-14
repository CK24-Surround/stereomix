// Copyright Surround, Inc. All Rights Reserved.


#include "SMLoginWidget.h"

#include "CommonTextBlock.h"
#include "StereoMixLog.h"
#include "Backend/SMGrpcGameSubsystem.h"
#include "GameInstance/SMGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "UI/Widget/Frontend/SMFrontendWidget.h"
#include "UI/Widget/Popup/SMAlertPopup.h"
#include "Utilities/SMVersion.h"

USMLoginWidget::USMLoginWidget()
{
	bIsBackHandler = false;
}

void USMLoginWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ProgressTextBlock->SetText(FText::GetEmpty());

	GrpcAuthSubsystem = GetGameInstance()->GetSubsystem<USMClientAuthSubsystem>();
	GrpcAuthSubsystem->OnServiceStateChanged.AddDynamic(this, &USMLoginWidget::OnAuthServiceStateChanged);
	GrpcAuthSubsystem->OnLoginResponse.AddDynamic(this, &USMLoginWidget::OnLoginResponse);

	GrpcGameSubsystem = GetGameInstance()->GetSubsystem<USMGrpcGameSubsystem>();
	GameService = GrpcGameSubsystem->GetGameService();
	GameService->OnServiceStateChanged.AddDynamic(this, &USMLoginWidget::OnGameServiceStateChanged);
	GameServiceClient = GameService->MakeClient();
	GameServiceClient->OnGetServiceVersionResponse.AddDynamic(this, &USMLoginWidget::OnGetServiceVersionResponse);
}

void USMLoginWidget::NativeDestruct()
{
	Super::NativeDestruct();
	if (GrpcAuthSubsystem)
	{
		GrpcAuthSubsystem->OnServiceStateChanged.RemoveDynamic(this, &USMLoginWidget::OnAuthServiceStateChanged);
		GrpcAuthSubsystem->OnLoginResponse.RemoveDynamic(this, &USMLoginWidget::OnLoginResponse);
	}
}

void USMLoginWidget::NativeOnActivated()
{
	Super::NativeOnActivated();

	GetParentFrontendWidget()->ClearPopups();
	if (GrpcGameSubsystem)
	{
		if (GrpcGameSubsystem->GetGameService()->GetServiceState() == EGrpcServiceState::Ready)
		{
			OnGameServiceStateChanged(EGrpcServiceState::Ready);
		}
		else
		{
			SetUiState(ELoginUiState::Connecting);
			GetWorld()->GetTimerManager().SetTimerForNextTick([&] { GrpcGameSubsystem->GetGameService()->Connect(); });
		}
	}
}

void USMLoginWidget::NativeOnDeactivated()
{
	Super::NativeOnDeactivated();
}

void USMLoginWidget::SetUiState(ELoginUiState NewState)
{
	UiState = NewState;

	switch (NewState)
	{
		case ELoginUiState::Idle:
			GetParentFrontendWidget()->ChangeBackgroundColor(BackgroundColor);
			ProgressTextBlock->SetText(FText::GetEmpty());
			break;
		case ELoginUiState::Connecting:
			GetParentFrontendWidget()->ChangeBackgroundColor(BackgroundColor);
			ProgressTextBlock->SetText(FText::FromString(TEXT("서버에 연결 중입니다...")));
			break;
		case ELoginUiState::VersionCheck:
			GetParentFrontendWidget()->ChangeBackgroundColor(BackgroundColor);
			ProgressTextBlock->SetText(FText::FromString(TEXT("게임 버전을 확인 중입니다...")));
			break;
		case ELoginUiState::Login:
			GetParentFrontendWidget()->ChangeBackgroundColor(BackgroundColor);
			ProgressTextBlock->SetText(FText::GetEmpty());
			break;
		case ELoginUiState::ProcessFailed:
			GetParentFrontendWidget()->ChangeBackgroundColor(FLinearColor(FColor(151, 36, 36)));
			ProgressTextBlock->SetText(FText::GetEmpty());
			break;
	}
}

void USMLoginWidget::OnGameServiceStateChanged(EGrpcServiceState ServiceState)
{
	if (ServiceState == EGrpcServiceState::Ready)
	{
		SetUiState(ELoginUiState::VersionCheck);

		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&] {
			if (GameServiceClient)
			{
				GrpcGetServiceVersionContextHandle = GameServiceClient->InitGetServiceVersion();
				GameServiceClient->GetServiceVersion(GrpcGetServiceVersionContextHandle, FGrpcGameGetServiceVersionRequest());
			}
		}, 0.5f, false);
	}
	else if (ServiceState == EGrpcServiceState::TransientFailure)
	{
		SetUiState(ELoginUiState::ProcessFailed);
		USMPopup* Popup = GetParentFrontendWidget()->ShowAlert(TEXT("서버와의 연결에 실패했습니다."));
		Popup->OnSubmit.BindWeakLambda(this, [&](USMPopup*) {
			GetWorld()->GetTimerManager().SetTimerForNextTick([this] { GameService->Connect(); });
		});
	}
}

void USMLoginWidget::OnGetServiceVersionResponse(FGrpcContextHandle Handle, const FGrpcResult& GrpcResult, const FGrpcGameGetServiceVersionResponse& Response)
{
	if (GrpcResult.Code != EGrpcResultCode::Ok)
	{
		GetParentFrontendWidget()->ClearPopups();
		SetUiState(ELoginUiState::ProcessFailed);
		UE_LOG(LogStereoMix, Error, TEXT("[SMLoginWidget] GetServiceVersion failed: %s, %s"), *GrpcResult.GetCodeString(), *GrpcResult.GetMessageString())
		GetParentFrontendWidget()->ShowAlert(TEXT("게임 버전을 체크하는 중 에러가 발생했습니다."))->OnSubmit.BindWeakLambda(this, [&](USMPopup*) {
			UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false);
		});
		return;
	}

	const FSMVersion ServiceVersion = FSMVersion(Response.Version);
	const FSMVersion ClientVersion = FSMVersion(GetGameInstance<USMGameInstance>()->GetGameVersion());

	UE_LOG(LogStereoMixUI, Log, TEXT("[SMLoginWidget] Service Version: %s, Client Version: %s"), *ServiceVersion.ToString(), *ClientVersion.ToString())
	if (ClientVersion < ServiceVersion)
	{
		UE_LOG(LogStereoMixUI, Warning, TEXT("[SMLoginWidget] Client version is lower than service version."))
		const FString Message = FString::Printf(TEXT("새로운 업데이트가 존재합니다. 홈페이지에서 새로운 버전을 다운로드 받아주세요.\n\n클라이언트 버전: %s\n최신 버전: %s"), *ClientVersion.ToString(), *ServiceVersion.ToString());
		GetParentFrontendWidget()->ShowAlert(Message)->OnSubmit.BindWeakLambda(this, [&](USMPopup*) {
			UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false);
		});
		return;
	}

	SetUiState(ELoginUiState::Login);
	if (GrpcAuthSubsystem)
	{
		GrpcAuthSubsystem->Connect();
	}
}

void USMLoginWidget::OnAuthServiceStateChanged(EGrpcServiceState ServiceState)
{
	if (ServiceState == EGrpcServiceState::Ready)
	{
		GetParentFrontendWidget()->ClearPopups();
		SetUiState(ELoginUiState::Login);
		USMPopup* Popup = GetParentFrontendWidget()->AddPopup(GuestLoginPopupClass.Get());
		Popup->OnSubmit.BindUObject(this, &USMLoginWidget::OnSubmitGuestLogin);
	}
	else if (ServiceState == EGrpcServiceState::TransientFailure)
	{
		SetUiState(ELoginUiState::ProcessFailed);
		USMPopup* Popup = GetParentFrontendWidget()->ShowAlert(TEXT("서버와의 연결에 실패했습니다."));
		Popup->OnSubmit.BindWeakLambda(this, [&](USMPopup*) {
			GetWorld()->GetTimerManager().SetTimerForNextTick([this] { GrpcAuthSubsystem->Connect(); });
		});
	}
}

void USMLoginWidget::OnLoginResponse(ELoginResult Result)
{
	switch (Result)
	{
		case ELoginResult::Success:
			UGameplayStatics::OpenLevelBySoftObjectPtr(this, GetWorld(), false, TEXT("init=mainmenu"));
			break;

		case ELoginResult::InvalidPassword:
			SetUiState(ELoginUiState::ProcessFailed);
			GetParentFrontendWidget()->ShowAlert(TEXT("비밀번호가 틀립니다."))->OnSubmit.BindWeakLambda(this, [&](USMPopup*) {
				GetParentFrontendWidget()->AddPopup(GuestLoginPopupClass.Get());
			});
			break;

		case ELoginResult::UnknownError:
		case ELoginResult::InvalidArgument:
		case ELoginResult::InternalError:
			SetUiState(ELoginUiState::ProcessFailed);
			GetParentFrontendWidget()->ShowAlert(TEXT("내부 오류로 인해 로그인하지 못했습니다."))->OnSubmit.BindWeakLambda(this, [&](USMPopup*) {
				UGameplayStatics::OpenLevelBySoftObjectPtr(this, GetWorld());
			});
			break;

		case ELoginResult::ConnectionError:
			SetUiState(ELoginUiState::ProcessFailed);
			GetParentFrontendWidget()->ShowAlert(TEXT("서버와의 연결에 실패했습니다."))->OnSubmit.BindWeakLambda(this, [&](USMPopup*) {
				UGameplayStatics::OpenLevelBySoftObjectPtr(this, GetWorld());
			});
			break;
	}
}

void USMLoginWidget::OnSubmitGuestLogin(USMPopup* Popup)
{
	if (ensure(Popup))
	{
		const USMGuestLoginPopup* GuestLoginPopup = CastChecked<USMGuestLoginPopup>(Popup);
		const FString UserName = GuestLoginPopup->GetUserName().ToString();
		if (GrpcAuthSubsystem)
		{
			GrpcAuthSubsystem->LoginAsGuest(UserName);
		}
	}
}
