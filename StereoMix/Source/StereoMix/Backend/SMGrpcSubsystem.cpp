// Copyright Surround, Inc. All Rights Reserved.


#include "SMGrpcSubsystem.h"

#include "StereoMix.h"
#include "StereoMixLog.h"
#include "TurboLinkGrpcConfig.h"
#include "TurboLinkGrpcManager.h"
#include "Kismet/GameplayStatics.h"

void USMGrpcSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	ensureMsgf(!ServerRootCerts.IsEmpty(), TEXT("ServerRootCerts must be specified in the config file."));
	ensureMsgf(!LiveServerEndPoint.IsEmpty(), TEXT("LiveServerEndPoint must be specified in the config file."));
	ensureMsgf(!DevelopmentServerEndPoint.IsEmpty(), TEXT("DevelopmentServerEndPoint must be specified in the config file."));
	ensureMsgf(!LocalServerEndPoint.IsEmpty(), TEXT("LocalServerEndPoint must be specified in the config file."));

	GrpcManager = Collection.InitializeDependency<UTurboLinkGrpcManager>();

	EConnectionEndPointTargets Target = EConnectionEndPointTargets::Local;
	if (!UGameplayStatics::HasLaunchOption("local"))
	{
		#if WITH_EDITOR
		Target = EConnectionEndPointTargets::Local;
		#elif UE_BUILD_DEVELOPMENT
		Target = EConnectionEndPointTargets::Live;
		#elif UE_BUILD_SHIPPING
		Target = EConnectionEndPointTargets::Live;
		#endif
	}
	SetTargetEndPoint(Target);
}

void USMGrpcSubsystem::SetTargetEndPoint(const EConnectionEndPointTargets Target)
{
	UE_LOG(LogStereoMix, Verbose, TEXT("[SMGrpcSubsystem] SetTargetEndPoint: %s"), *UEnum::GetValueAsString(Target))

	UTurboLinkGrpcConfig* TurboLinkConfig = GetMutableDefault<UTurboLinkGrpcConfig>();
	// 로컬을 제외하고는 모두 SecureChannel을 사용해야 합니다.
	switch (Target)
	{
		case EConnectionEndPointTargets::Live:
			TurboLinkConfig->EnableServerSideTLS = true;
			TurboLinkConfig->ServerRootCerts = ServerRootCerts;
			TurboLinkConfig->DefaultEndPoint = LiveServerEndPoint;
			break;

		case EConnectionEndPointTargets::Development:
			TurboLinkConfig->EnableServerSideTLS = true;
			TurboLinkConfig->ServerRootCerts = ServerRootCerts;
			TurboLinkConfig->DefaultEndPoint = LiveServerEndPoint;
			break;

		case EConnectionEndPointTargets::Local:
			// 임시 코드
			// TurboLinkConfig->EnableServerSideTLS = true;
			// TurboLinkConfig->ServerRootCerts = ServerRootCerts;
			// TurboLinkConfig->DefaultEndPoint = LiveServerEndPoint;
			TurboLinkConfig->EnableServerSideTLS = false;
			TurboLinkConfig->ServerRootCerts = TEXT("");
			TurboLinkConfig->DefaultEndPoint = LocalServerEndPoint;
			break;
	}
}