// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Backend/SMAuthSubsystem.h"
#include "SAuth/AuthMessage.h"
#include "SAuth/AuthService.h"
#include "SMUserAccount.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "SMClientAuthSubsystem.generated.h"

struct FGrpcContextHandle;

UENUM(BlueprintType)
enum class ELoginResult : uint8
{
	Success,
	UnknownError,
	InvalidArgument,
	InvalidPassword,
	InternalError,
	ConnectionError
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLoginResponseDelegate, ELoginResult, Result);

/**
 * StereoMix Authentication Subsystem for Client
 */
UCLASS()
class STEREOMIX_API USMClientAuthSubsystem : public USMAuthSubsystem
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FLoginResponseDelegate OnLoginResponse;

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/**
	 * 현재 클라이언트가 서버로부터 인증되었는지 여부를 가져옵니다.
	 * @return 클라이언트의 인증 여부
	 */
	virtual bool IsAuthenticated() const override { return UserAccount != nullptr; }

	/**
	 * 사용자 계정을 가져옵니다. 로그인되지 않은 경우 nullptr을 반환합니다.
	 * @return 사용자 계정
	 */
	UFUNCTION(BlueprintPure)
	USMUserAccount* GetUserAccount() const { return UserAccount; }

	UFUNCTION(BlueprintCallable)
	void ResetAccount();

	/**
	 * 게스트 계정으로 로그인합니다. 로그인 완료 시 OnLoginResponse 이벤트가 호출됩니다.
	 * @param UserName 게스트 이름
	 * @return 로그인 요청이 성공적으로 시작되었는지 여부를 반환합니다. 서브시스템이 아직 초기화되지 않았거나 다른 로그인 요청이 이미 진행 중인 경우 false를 반환합니다.
	 */
	UFUNCTION(BlueprintCallable)
	bool LoginAsGuest(const FString& UserName);

protected:
	FGrpcContextHandle GrpcContextHandle;

	UPROPERTY()
	TObjectPtr<UAuthServiceClient> AuthClient;

	UPROPERTY()
	TObjectPtr<USMUserAccount> UserAccount;

private:
	UFUNCTION()
	void OnGrpcLoginResponse(FGrpcContextHandle Handle, const FGrpcResult& Result, const FGrpcAuthLoginResponse& Response);

	static ELoginResult GetLoginFailReason(EGrpcResultCode Code);

	virtual bool IsBusy() const override;

	void InitUserAccount(const FString& UserAccessToken, const FString& UserRefreshToken, const FGrpcAuthUserAccount& GrpcUserAccount);
};
