// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TurboLinkGrpcClient.h"
#include "UObject/Object.h"
#include "SMUserAccount.generated.h"

struct FGrpcMetaData;
struct FGrpcAuthUserAccount;

/**
 * StereoMix User Account
 */
UCLASS(BlueprintType)
class STEREOMIX_API USMUserAccount : public UObject
{
	GENERATED_BODY()

public:
	/** 사용자의 액세스 토큰을 반환합니다. */
	UFUNCTION(BlueprintCallable)
	const FString& GetAccessToken() const { return AccessToken; }

	/** 사용자의 리프레시 토큰을 반환합니다. */
	UFUNCTION(BlueprintCallable)
	const FString& GetRefreshToken() const { return RefreshToken; }
	
	/** 사용자의 아이디를 반환합니다. */
	UFUNCTION(BlueprintCallable)
	const FString& GetUserId() const { return UserId; }

	/** 사용자의 이름을 반환합니다. */
	UFUNCTION(BlueprintCallable)
	const FString& GetUserName() const { return UserName; }

	/**
	 * 사용자 계정을 초기화합니다.
	 * @param UserAccessToken 액세스 토큰
	 * @param UserRefreshToken 리프레시 토큰
	 * @param GrpcUserAccount gRPC 사용자 계정 정보
	 */
	void Init(const FString& UserAccessToken, const FString& UserRefreshToken, const FGrpcAuthUserAccount& GrpcUserAccount);

	/** 사용자의 인증 헤더를 반환합니다. */
	UFUNCTION(BlueprintPure)
	const FGrpcMetaData& GetAuthorizationHeader() const { return AuthorizationHeader; }

private:
	UPROPERTY(VisibleDefaultsOnly, Category="UserAccount", meta=(AllowPrivateAccess="true"))
	FString AccessToken;

	UPROPERTY(VisibleDefaultsOnly, Category="UserAccount", meta=(AllowPrivateAccess="true"))
	FString RefreshToken;
	
	UPROPERTY(VisibleDefaultsOnly, Category="UserAccount", meta=(AllowPrivateAccess="true"))
	FString UserId;
	
	UPROPERTY(VisibleDefaultsOnly, Category="UserAccount", meta=(AllowPrivateAccess="true"))
	FString UserName;

	FGrpcMetaData AuthorizationHeader;
};
