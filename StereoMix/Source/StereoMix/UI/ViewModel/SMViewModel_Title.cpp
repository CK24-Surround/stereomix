// Copyright Surround, Inc. All Rights Reserved.


#include "SMViewModel_Title.h"

USMViewModel_Title::USMViewModel_Title()
{
	ConnectionStatusText = FText::GetEmpty();
}

void USMViewModel_Title::SetConnectionStatus(const ETitleConnectionStatus NewStatus)
{
	switch (NewStatus)
	{
	case ETitleConnectionStatus::Connecting:
		SetConnectionStatusText(FText::FromName(TEXT("서버와 연결 중 입니다...")));
		break;
	case ETitleConnectionStatus::Connected:
		SetConnectionStatusText(FText::FromName(TEXT("서버와 성공적으로 연결되었습니다.")));
		break;
	case ETitleConnectionStatus::ConnectionFailed:
		SetConnectionStatusText(FText::FromName(TEXT("연결에 실패했습니다. 재 연결 시도 중 입니다...")));
		break;
	case ETitleConnectionStatus::LoggingIn:
		SetConnectionStatusText(FText::FromName(TEXT("로그인 중...")));
		break;
	case ETitleConnectionStatus::LoginSuccess:
		SetConnectionStatusText(FText::FromName(TEXT("로그인 성공!")));
		break;
	case ETitleConnectionStatus::LoginFailed:
		SetConnectionStatusText(FText::FromName(TEXT("로그인에 실패했습니다.")));
		break;
	case ETitleConnectionStatus::None:
	default:
		SetConnectionStatusText(FText::FromName(TEXT("")));
		break;
	}
}
