// Copyright Surround, Inc. All Rights Reserved.


#include "SMGrpcSubsystem.h"

#include "TurboLinkGrpcConfig.h"
#include "TurboLinkGrpcUtilities.h"
#include "Kismet/GameplayStatics.h"

void USMGrpcSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UTurboLinkGrpcManager* TurboLinkManager = UTurboLinkGrpcUtilities::GetTurboLinkGrpcManager(this);

	EGrpcServiceEndPointTargets Target = EGrpcServiceEndPointTargets::Local;

	if (!UGameplayStatics::HasLaunchOption("local"))
	{
#if UE_BUILD_DEVELOPMENT // 개발 서버
		Target = EGrpcServiceEndPointTargets::Development;
#elif UE_BUILD_SHIPPING // 라이브 서버
		Target = EGrpcServiceEndPointTargets::Production;
#endif
	}

	SetTargetEndPoint(Target);
	GrpcManager = UTurboLinkGrpcUtilities::GetTurboLinkGrpcManager(this);
}

void USMGrpcSubsystem::SetTargetEndPoint(const EGrpcServiceEndPointTargets Target)
{
	UTurboLinkGrpcConfig* TurboLinkConfig = GetMutableDefault<UTurboLinkGrpcConfig>();

	UE_LOG(LogSMGrpcSubsystem, Log, TEXT("Set gRPC EndPoint to %s"), *UEnum::GetValueAsString(Target))

	// 로컬을 제외하고는 모두 SecureChannel을 사용해야 합니다.
	switch (Target)
	{
	case EGrpcServiceEndPointTargets::Production:
		TurboLinkConfig->EnableServerSideTLS = true;
		TurboLinkConfig->ServerRootCerts = ServerRootCerts;
		TurboLinkConfig->DefaultEndPoint = ProductionGrpcEndPoint;
		break;

	case EGrpcServiceEndPointTargets::Development:
		TurboLinkConfig->EnableServerSideTLS = true;
		TurboLinkConfig->ServerRootCerts = ServerRootCerts;
		TurboLinkConfig->DefaultEndPoint = DevelopmentGrpcEndPoint;
		break;

	case EGrpcServiceEndPointTargets::Local:
		// 로컬 테스트의 편의를 위해 SecureChannel을 사용하지 않습니다.
		TurboLinkConfig->EnableServerSideTLS = false;
		TurboLinkConfig->ServerRootCerts = TEXT("");
		TurboLinkConfig->DefaultEndPoint = LocalGrpcEndPoint;
		break;
	}
}
