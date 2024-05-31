// Copyright Surround, Inc. All Rights Reserved.


#include "SMTitleViewModel.h"

#include "StereoMix.h"
#include "Backend/Client/SMClientAuthSubsystem.h"
#include "Kismet/GameplayStatics.h"

USMTitleViewModel::USMTitleViewModel()
{
	UiState = ETitleUiState::Idle;
}

void USMTitleViewModel::InitializeViewModel(UWorld* InWorld)
{
	Super::InitializeViewModel(InWorld);

	if (!InWorld)
	{
		return;
	}

	AuthSubsystem = InWorld->GetGameInstance()->GetSubsystem<USMClientAuthSubsystem>();
	if (AuthSubsystem->IsAuthenticated())
	{
		SetUiState(ETitleUiState::LoginSuccess);
		return;
	}

	AuthSubsystem->GetAuthService()->OnServiceStateChanged.AddDynamic(this, &USMTitleViewModel::OnServiceStateChanged);
	AuthSubsystem->OnLoginResponse.AddDynamic(this, &USMTitleViewModel::OnLoginResponse);

	
	AuthSubsystem->Connect();
}

void USMTitleViewModel::BeginDestroy()
{
	Super::BeginDestroy();

	if (AuthSubsystem.IsValid())
	{
		AuthSubsystem->GetAuthService()->OnServiceStateChanged.RemoveDynamic(this, &USMTitleViewModel::OnServiceStateChanged);
		AuthSubsystem->OnLoginResponse.RemoveDynamic(this, &USMTitleViewModel::OnLoginResponse);
	}
}

void USMTitleViewModel::SetUiState(const ETitleUiState NewState)
{
	UE_MVVM_SET_PROPERTY_VALUE(UiState, NewState);
}

void USMTitleViewModel::OnServiceStateChanged(const EGrpcServiceState ServiceState)
{
	switch (ServiceState) {
	case EGrpcServiceState::Connecting:
		SetUiState(ETitleUiState::Connecting);
		break;
		
	case EGrpcServiceState::Ready:
		SetUiState(ETitleUiState::Connected);
		break;
		
	case EGrpcServiceState::TransientFailure:
		SetUiState(ETitleUiState::ConnectionFailed);
		break;
		
	default:
		break;
	}
}

void USMTitleViewModel::OnLoginResponse(const ELoginResult Result)
{
	switch (Result) {
	case ELoginResult::Success:
		SetUiState(ETitleUiState::LoginSuccess);
		break;
		
	case ELoginResult::UnknownError:
	case ELoginResult::InvalidArgument:
	case ELoginResult::InvalidPassword:
	case ELoginResult::InternalError:
	case ELoginResult::ConnectionError:
		UE_LOG(LogStereoMix, Error, TEXT("Login failed: %s"), *UEnum::GetValueAsString(Result));
		SetUiState(ETitleUiState::LoginFailed);
		break;
	}
}

void USMTitleViewModel::GoToMainMenu()
{
	GetWorld()->GetGameInstance()->level
}
