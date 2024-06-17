// Copyright Surround, Inc. All Rights Reserved.


#include "SMLoginWidget.h"

#include "UI/Widget/Frontend/SMFrontendWidget.h"
#include "Kismet/GameplayStatics.h"
#include "UI/Widget/Popup/SMAlertPopup.h"

USMLoginWidget::USMLoginWidget()
{
	bIsBackHandler = false;
}

void USMLoginWidget::NativeConstruct()
{
	Super::NativeConstruct();

	AuthSubsystem = GetGameInstance()->GetSubsystem<USMClientAuthSubsystem>();
}

void USMLoginWidget::NativeOnActivated()
{
	Super::NativeOnActivated();

	LoginPopupStack->ClearWidgets();
	if (AuthSubsystem)
	{
		AuthSubsystem->OnServiceStateChanged.AddDynamic(this, &USMLoginWidget::OnAuthServiceStateChanged);
		AuthSubsystem->OnLoginResponse.AddDynamic(this, &USMLoginWidget::OnLoginResponse);

		if (AuthSubsystem->GetAuthService()->GetServiceState() == EGrpcServiceState::Ready)
		{
			OnAuthServiceStateChanged(EGrpcServiceState::Ready);
		}
		else
		{
			GetWorld()->GetTimerManager().SetTimerForNextTick([this]
			{
				AuthSubsystem->Connect();
			});
		}
	}
}

void USMLoginWidget::NativeOnDeactivated()
{
	Super::NativeOnDeactivated();

	if (AuthSubsystem)
	{
		AuthSubsystem->OnServiceStateChanged.RemoveDynamic(this, &USMLoginWidget::OnAuthServiceStateChanged);
		AuthSubsystem->OnLoginResponse.RemoveDynamic(this, &USMLoginWidget::OnLoginResponse);
	}

	if (GuestLoginPopup)
	{
		GuestLoginPopup->OnSubmit.Unbind();
	}
}

void USMLoginWidget::OnAuthServiceStateChanged(EGrpcServiceState ServiceState)
{
	if (ServiceState == EGrpcServiceState::Ready && LoginPopupStack->GetNumWidgets() == 0)
	{
		if (ConnectFailedPopup.IsValid())
		{
			ConnectFailedPopup->DeactivateWidget();
			ConnectFailedPopup.Reset();
		}
		GuestLoginPopup = LoginPopupStack->AddWidget<USMGuestLoginPopup>(GuestLoginPopupClass);
		GuestLoginPopup->OnSubmit.BindUObject(this, &USMLoginWidget::OnSubmitGuestLogin);
	}
	else if (ServiceState == EGrpcServiceState::TransientFailure && !ConnectFailedPopup.IsValid())
	{
		ConnectFailedPopup = GetParentFrontendWidget()->ShowAlert(TEXT("서버와의 연결에 실패했습니다."));
		ConnectFailedPopup->OnSubmit.BindWeakLambda(this, [&]
		{
			// UGameplayStatics::OpenLevelBySoftObjectPtr(this, GetWorld());

			// 연속 시도 시 로그인 창이 중복으로 뜨는 버그가 있을 수 있음 
			GetWorld()->GetTimerManager().SetTimerForNextTick([this]
			{
				AuthSubsystem->Connect();
			});
			ConnectFailedPopup.Reset();
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
			GetParentFrontendWidget()->ShowAlert(TEXT("비밀번호가 틀립니다."))->OnSubmit.BindWeakLambda(this, [&]
			{
				GuestLoginPopup = LoginPopupStack->AddWidget<USMGuestLoginPopup>(GuestLoginPopupClass);
			});
			break;

		case ELoginResult::UnknownError:
		case ELoginResult::InvalidArgument:
		case ELoginResult::InternalError:
			GetParentFrontendWidget()->ShowAlert(TEXT("내부 오류로 인해 로그인하지 못했습니다."))->OnSubmit.BindWeakLambda(this, [&]
			{
				UGameplayStatics::OpenLevelBySoftObjectPtr(this, GetWorld());
			});
			break;

		case ELoginResult::ConnectionError:
			GetParentFrontendWidget()->ShowAlert(TEXT("서버와의 연결에 실패했습니다."))->OnSubmit.BindWeakLambda(this, [&]
			{
				UGameplayStatics::OpenLevelBySoftObjectPtr(this, GetWorld());
			});
			break;
	}
}

void USMLoginWidget::OnSubmitGuestLogin()
{
	const FString UserName = GuestLoginPopup->GetUserName().ToString();
	if (AuthSubsystem)
	{
		AuthSubsystem->LoginAsGuest(UserName);
	}
}
