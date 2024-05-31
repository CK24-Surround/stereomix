#include "SMUserAccount.h"

#include "SAuth/AuthMessage.h"

void USMUserAccount::Init(const FString& UserAccessToken, const FString& UserRefreshToken, const FGrpcAuthUserAccount& GrpcUserAccount)
{
	AccessToken = UserAccessToken;
	RefreshToken = UserRefreshToken;
	UserId = GrpcUserAccount.UserId;
	UserName = GrpcUserAccount.UserName;

	AuthorizationHeader = FGrpcMetaData();
	AuthorizationHeader.MetaData.Add(TEXT("authorization"), TEXT("Bearer ") + UserAccessToken);
}
