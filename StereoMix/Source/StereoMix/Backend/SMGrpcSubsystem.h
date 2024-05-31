// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SMGrpcSubsystem.generated.h"

class UTurboLinkGrpcManager;

UENUM(BlueprintType)
enum class EConnectionEndPointTargets : uint8
{
	Live,
	Development,
	Local
};

/**
 * StereoMix gRPC Subsystem
 */
UCLASS(BlueprintType, Config=Game, DefaultConfig)
class STEREOMIX_API USMGrpcSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UTurboLinkGrpcManager* GetGrpcManager() const { return GrpcManager; }
	
protected:
	/**
	 * gRPC 서비스에 접속할 엔드포인트를 설정합니다.
	 * 
	 * @param Target gRPC 서비스 엔드포인트 타겟
	 */
	void SetTargetEndPoint(EConnectionEndPointTargets Target);

private:
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

	UPROPERTY()
	TObjectPtr<UTurboLinkGrpcManager> GrpcManager;
};
