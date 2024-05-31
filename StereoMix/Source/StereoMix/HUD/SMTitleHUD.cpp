// Copyright Surround, Inc. All Rights Reserved.


#include "SMTitleHUD.h"

#include "StereoMix.h"
#include "Blueprint/UserWidget.h"
#include "Connection/SMClientConnectionSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "UI/ViewModel/SMGuestLoginViewModel.h"
#include "UI/ViewModel/SMTitleViewModel.h"

ASMTitleHUD::ASMTitleHUD()
{
}

void ASMTitleHUD::BeginPlay()
{
	Super::BeginPlay();

	if (MainWidget)
	{
		UMVVMView* MainWidgetView = UMVVMSubsystem::GetViewFromUserWidget(MainWidget);
	}
}

void ASMTitleHUD::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (const auto ConnectionSubsystem = GetGameInstance()->GetSubsystem<USMClientConnectionSubsystem>())
	{
		ConnectionSubsystem->AuthServiceStateChangedEvent.RemoveDynamic(this, &ASMTitleHUD::OnAuthServiceStateChanged);
		ConnectionSubsystem->LoginEvent.RemoveDynamic(this, &ASMTitleHUD::OnLogin);
	}
}

void ASMTitleHUD::CreateGuestLoginWidget()
{
	checkfSlow(GuestLoginWidgetClass, TEXT("GuestLoginWidgetClass is not set."))
	GuestLoginWidget = AddWidgetToViewport(GuestLoginWidgetClass);
	if (GuestLoginWidget)
	{
		if (auto View = GetViewModelFromWidget<USMGuestLoginViewModel>(GuestLoginWidget, TEXT("SMViewModel_GuestLogin")))
		{
			GuestLoginViewModel = View;
			GuestLoginViewModel->OnSubmit.BindUObject(this, &ASMTitleHUD::OnGuestLoginSubmit);
		}
	}
}

void ASMTitleHUD::OnGuestLoginSubmit()
{
	if (!GuestLoginViewModel)
	{
		return;
	}

	if (const auto ConnectionSubsystem = GetGameInstance()->GetSubsystem<USMClientConnectionSubsystem>())
	{
		ConnectionSubsystem->GuestLogin(GuestLoginViewModel->GetUserName());
		GuestLoginWidget->RemoveFromParent();
	}
}

void ASMTitleHUD::OnAuthServiceStateChanged(const EGrpcServiceState ChangedState)
{
	if (!TitleViewModel)
	{
		return;
	}

	// TODO: ViewModel로 옮기기
	ETitleConnectionStatus ConnectionStatus = ETitleConnectionStatus::None;
	switch (ChangedState)
	{
	case EGrpcServiceState::NotCreate:
	case EGrpcServiceState::Shutdown:
		ConnectionStatus = ETitleConnectionStatus::None;
		break;
	case EGrpcServiceState::Idle:
	case EGrpcServiceState::Connecting:
		ConnectionStatus = ETitleConnectionStatus::Connecting;
		break;
	case EGrpcServiceState::Ready:
		ConnectionStatus = ETitleConnectionStatus::Connected;
		break;
	case EGrpcServiceState::TransientFailure:
		ConnectionStatus = ETitleConnectionStatus::ConnectionFailed;
		break;
	}
	
	TitleViewModel->SetConnectionStatus(ConnectionStatus);
	if (ChangedState == EGrpcServiceState::Ready)
	{
		CreateGuestLoginWidget();
	}
}

void ASMTitleHUD::OnLogin(const EGrpcResultCode Result)
{
	UE_LOG(LogStereoMix, Verbose, TEXT("Login result: %s"), *UEnum::GetValueAsString(Result))
	if (Result == EGrpcResultCode::Ok)
	{
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, this, &ASMTitleHUD::GoToMainMenu, 1.0f, false);
	}
	else
	{
		// TODO: 로그인 실패
	}
}

void ASMTitleHUD::GoToMainMenu()
{
	UGameplayStatics::OpenLevel(this, TEXT("L_MainMenu"));
}
