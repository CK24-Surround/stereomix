// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TurboLinkGrpcService.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SMConnectionSubsystem.generated.h"

struct FGrpcMetaData;
class UAuthService;
class UAuthServiceClient;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGrpcServiceStateChangedEvent, EGrpcServiceState, ChangedState);

UENUM()
enum class EConnectionEndPointTargets : uint8
{
	Live,
	Development,
	Local
};

/**
 * StereoMix Connection Subsystem
 */
UCLASS(Abstract, Config=Game, DefaultConfig)
class STEREOMIX_API USMConnectionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	USMConnectionSubsystem();

	// ===============================================================
	// UGameInstanceSubsystem

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;

	// ===============================================================
	// Connection EndPoint

protected:
	/**
	 * gRPC 서비스에 접속할 엔드포인트를 설정합니다.
	 * 
	 * @param Target gRPC 서비스 엔드포인트 타겟
	 */
	void SetTargetEndPoint(EConnectionEndPointTargets Target);


	// ----------------------------------------------
	// Configurations

	/** 서버 측 인증서 */
	UPROPERTY(Config)
	FString ServerRootCerts;

	/** 라이브 서버 엔드포인트 */
	UPROPERTY(Config)
	FString LiveServerEndPoint;

	/** 개발 서버 엔드포인트 */
	UPROPERTY(Config)
	FString DevelopmentServerEndPoint;

	/** 테스트용 로컬 엔드포인트 */
	UPROPERTY(Config)
	FString LocalServerEndPoint;


	// ===============================================================
	// gRPC AuthService

	bool bIsAuthenticated;

	/**
	 * gRPC AuthService 채널 인스턴스
	 */
	UPROPERTY(Transient)
	TObjectPtr<UAuthService> AuthService;

	/**
	 * gRPC AuthService 클라이언트 인스턴스
	 */
	UPROPERTY(Transient)
	TObjectPtr<UAuthServiceClient> AuthServiceClient;

	/**
	 * 인증 토큰
	 */
	FString AuthToken;

	FGrpcMetaData GetAuthorizationMetaData() const;

public:
	/**
	 * AuthService의 상태가 변경되었을 때 호출되는 이벤트
	 */
	UPROPERTY(BlueprintAssignable)
	FGrpcServiceStateChangedEvent AuthServiceStateChangedEvent;

	/**
	 * AuthService에 연결합니다.
	 */
	UFUNCTION(BlueprintCallable)
	void ConnectAuthService() const;

	UFUNCTION(BlueprintCallable)
	virtual bool IsAuthenticated() const;

	/**
	 * AuthService의 상태가 변경되었을 때 호출됩니다.
	 * @param ChangedState 변경된 상태
	 */
	UFUNCTION()
	virtual void OnAuthServiceStateChanged(EGrpcServiceState ChangedState);
};
