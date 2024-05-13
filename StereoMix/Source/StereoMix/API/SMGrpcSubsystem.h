// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TurboLinkGrpcManager.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SMGrpcSubsystem.generated.h"

DECLARE_LOG_CATEGORY_CLASS(LogSMGrpcSubsystem, Log, All)

UENUM()
enum class EGrpcServiceEndPointTargets : uint8
{
	Production,
	Development,
	Local
};

/**
 * StereoMix gRPC Subsystem
 */
UCLASS(Config=Game, DefaultConfig)
class STEREOMIX_API USMGrpcSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	/** 서버 측 인증서 */
	UPROPERTY(Config)
	FString ServerRootCerts;

	/** 정식 서버 엔드포인트 */
	UPROPERTY(Config)
	FString ProductionGrpcEndPoint;

	/** 개발 서버 엔드포인트 */
	UPROPERTY(Config)
	FString DevelopmentGrpcEndPoint;

	/** 테스트용 로컬 엔드포인트 */
	UPROPERTY(Config)
	FString LocalGrpcEndPoint;

	UPROPERTY(Transient)
	TObjectPtr<UTurboLinkGrpcManager> GrpcManager;
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	FORCEINLINE UTurboLinkGrpcManager* GetManager() const { return GrpcManager; }
	
	/**
	 * gRPC 서비스에 접속할 엔드포인트를 설정합니다.
	 * 
	 * @param Target gRPC 서비스 엔드포인트 타겟
	 */
	void SetTargetEndPoint(EGrpcServiceEndPointTargets Target);
};
