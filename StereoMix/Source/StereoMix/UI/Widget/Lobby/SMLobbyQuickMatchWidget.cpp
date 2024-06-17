// Copyright Surround, Inc. All Rights Reserved.


#include "SMLobbyQuickMatchWidget.h"

#include "UI/Widget/Frontend/SMFrontendWidget.h"
#include "StereoMixLog.h"
#include "Kismet/GameplayStatics.h"

USMLobbyQuickMatchWidget::USMLobbyQuickMatchWidget() {}

void USMLobbyQuickMatchWidget::NativeOnActivated()
{
	Super::NativeOnActivated();

	if (!CheckIsAuthenticated())
	{
		return;
	}

	if (LobbySubsystem)
	{
		LobbySubsystem->OnQuickMatchResponse.AddDynamic(this, &USMLobbyQuickMatchWidget::OnQuickMatchResponse);

		if (!LobbySubsystem->QuickMatch())
		{
			UiState = ELobbyProcessUiState::Failure;
			UE_LOG(LogStereoMixUI, Error, TEXT("[SMLobbyQuickMatchWidget] Failed to quick match"));
			GetParentFrontendWidget()->ShowAlert(TEXT("매칭에 실패했습니다."))->OnSubmit.BindWeakLambda(this, [&]
			{
				GetParentFrontendWidget()->RemoveElementWidget(this);
			});
			return;
		}

		UiState = ELobbyProcessUiState::Processing;
		SetProgressVisibility(ESlateVisibility::Visible);
		SetProgressText(FText::FromString(TEXT("열려있는 방을 찾고 있습니다...")));
	}
}

void USMLobbyQuickMatchWidget::NativeOnDeactivated()
{
	Super::NativeOnDeactivated();

	if (LobbySubsystem)
	{
		LobbySubsystem->OnQuickMatchResponse.RemoveDynamic(this, &USMLobbyQuickMatchWidget::OnQuickMatchResponse);
	}
}

void USMLobbyQuickMatchWidget::OnQuickMatchResponse(const EQuickMatchResult Result, const FString& ServerUrl)
{
	if (Result == EQuickMatchResult::Success)
	{
		UiState = ELobbyProcessUiState::Success;
		ConnectToGameServer(ServerUrl);
		return;
	}

	switch (Result)
	{
		case EQuickMatchResult::Cancelled:
			break;

		case EQuickMatchResult::Unauthenticated:
			GetParentFrontendWidget()->ShowAlert(TEXT("인증 정보가 유효하지 않습니다."))->OnSubmit.BindWeakLambda(this, [&]
			{
				UGameplayStatics::OpenLevelBySoftObjectPtr(this, GetWorld());
			});
			break;

		case EQuickMatchResult::ConnectionError:
			GetParentFrontendWidget()->ShowAlert(TEXT("서버와의 연결에 실패했습니다."))->OnSubmit.BindWeakLambda(this, [&]
			{
				UGameplayStatics::OpenLevelBySoftObjectPtr(this, GetWorld());
			});
			break;

		case EQuickMatchResult::InvalidArgument:
		case EQuickMatchResult::UnknownError:
		case EQuickMatchResult::InternalError:
		case EQuickMatchResult::DeadlineExceeded:
		default:
			GetParentFrontendWidget()->ShowAlert(TEXT("매칭에 실패했습니다."))->OnSubmit.BindWeakLambda(this, [&]
			{
				GetParentFrontendWidget()->RemoveElementWidget(this);
			});
			break;
	}

	UiState = ELobbyProcessUiState::Failure;
	SetProgressVisibility(ESlateVisibility::Collapsed);
}
