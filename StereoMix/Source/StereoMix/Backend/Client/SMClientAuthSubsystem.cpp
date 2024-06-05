// Copyright Surround, Inc. All Rights Reserved.


#include "SMClientAuthSubsystem.h"

#include "SMUserAccount.h"
#include "StereoMix.h"
#include "StereoMixLog.h"
#include "SAuth/AuthService.h"

bool USMClientAuthSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return true;
}

void USMClientAuthSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	AuthClient = GetAuthService()->MakeClient();
	AuthClient->OnGuestLoginResponse.AddUniqueDynamic(this, &USMClientAuthSubsystem::OnGrpcLoginResponse);
}

void USMClientAuthSubsystem::ResetAccount()
{
	UserAccount = nullptr;
}

bool USMClientAuthSubsystem::LoginAsGuest(const FString& UserName)
{
	if (!AuthClient || IsBusy())
	{
		return false;
	}

	GrpcContextHandle = AuthClient->InitGuestLogin();
	FGrpcAuthGuestLoginRequest Request;
	Request.UserName = UserName;

	AuthClient->GuestLogin(GrpcContextHandle, Request);
	return true;
}

void USMClientAuthSubsystem::OnGrpcLoginResponse(const FGrpcContextHandle Handle, const FGrpcResult& Result, const FGrpcAuthLoginResponse& Response)
{
	if (Handle != GrpcContextHandle)
	{
		return;
	}

	if (Result.Code != EGrpcResultCode::Ok)
	{
		UE_LOG(LogStereoMix, Error, TEXT("[SMClientAuthSubsystem] login failed: %s, %s"), *Result.GetCodeString(), *Result.GetMessageString())
		if (OnLoginResponse.IsBound())
		{
			OnLoginResponse.Broadcast(GetLoginFailReason(Result.Code));
		}
		return;
	}

	UE_LOG(LogStereoMix, Log, TEXT("[SMClientAuthSubsystem] User logged in successfully. UserId: %s, UserName: %s"), *Response.UserAccount.UserId, *Response.UserAccount.UserName)

	InitUserAccount(Response.AccessToken, Response.RefreshToken, Response.UserAccount);
	if (OnLoginResponse.IsBound())
	{
		OnLoginResponse.Broadcast(ELoginResult::Success);
	}
}

bool USMClientAuthSubsystem::IsBusy() const
{
	bool bBusy = AuthClient->GetContextState(GrpcContextHandle) == EGrpcContextState::Busy;
	if (bBusy)
	{
		UE_LOG(LogStereoMix, Warning, TEXT("[SMClientAuthSubsystem] 다른 요청이 이미 진행 중입니다."))
	}
	return bBusy;
}

ELoginResult USMClientAuthSubsystem::GetLoginFailReason(const EGrpcResultCode Code)
{
	switch (Code)
	{
	case EGrpcResultCode::InvalidArgument:
		return ELoginResult::InvalidArgument;

	case EGrpcResultCode::Internal:
		return ELoginResult::InternalError;

	case EGrpcResultCode::Unauthenticated:
		return ELoginResult::InvalidPassword;

	case EGrpcResultCode::DeadlineExceeded:
	case EGrpcResultCode::ConnectionFailed:
		return ELoginResult::ConnectionError;

	default:
		return ELoginResult::UnknownError;
	}
}

void USMClientAuthSubsystem::InitUserAccount(const FString& UserAccessToken, const FString& UserRefreshToken, const FGrpcAuthUserAccount& GrpcUserAccount)
{
	UserAccount = NewObject<USMUserAccount>();
	UserAccount->Init(UserAccessToken, UserRefreshToken, GrpcUserAccount);
}
