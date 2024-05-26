// Copyright Surround, Inc. All Rights Reserved.


#include "SMConnectionSubsystem.h"

#include "StereoMix.h"
#include "TurboLinkGrpcConfig.h"
#include "TurboLinkGrpcManager.h"
#include "TurboLinkGrpcUtilities.h"
#include "FunctionLibraries/SMGrpcServiceUtil.h"
#include "Kismet/GameplayStatics.h"
#include "SAuth/AuthService.h"

DEFINE_LOG_CATEGORY(LogStereoMix);

USMConnectionSubsystem::USMConnectionSubsystem()
{
	bIsAuthenticated = false;
}

bool USMConnectionSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return true;
}

void USMConnectionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	ensureMsgf(!ServerRootCerts.IsEmpty(), TEXT("ServerRootCerts must be specified in the config file."));
	ensureMsgf(!LiveServerEndPoint.IsEmpty(), TEXT("LiveServerEndPoint must be specified in the config file."));
	ensureMsgf(!DevelopmentServerEndPoint.IsEmpty(), TEXT("DevelopmentServerEndPoint must be specified in the config file."));
	ensureMsgf(!LocalServerEndPoint.IsEmpty(), TEXT("LocalServerEndPoint must be specified in the config file."));

	UTurboLinkGrpcManager* TurboLinkManager = GetGameInstance()->GetSubsystem<UTurboLinkGrpcManager>();

	EConnectionEndPointTargets Target = EConnectionEndPointTargets::Local;
	if (!UGameplayStatics::HasLaunchOption("local"))
	{
#if WITH_EDITOR
		Target = EConnectionEndPointTargets::Local;
#elif UE_BUILD_DEVELOPMENT
		Target = EConnectionEndPointTargets::Development;
#elif UE_BUILD_SHIPPING
		Target = EConnectionEndPointTargets::Live;
#endif
	}
	SetTargetEndPoint(Target);

	// Init AuthService
	AuthService = Cast<UAuthService>(TurboLinkManager->MakeService(TEXT("AuthService")));
	AuthService->OnServiceStateChanged.AddUniqueDynamic(this, &USMConnectionSubsystem::OnAuthServiceStateChanged);
	AuthServiceClient = AuthService->MakeClient();
}

void USMConnectionSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

FGrpcMetaData USMConnectionSubsystem::GetAuthorizationMetaData() const
{
	FGrpcMetaData Metadata;
	Metadata.MetaData.Add(TEXT("authorization"), TEXT("Bearer ") + AuthToken);
	UE_LOG(LogStereoMix, Verbose, TEXT("Authorization: %s"), *Metadata.MetaData.FindRef(TEXT("authorization")))
	return Metadata;
}

void USMConnectionSubsystem::ConnectAuthService() const
{
	check(AuthService)
	AuthService->Connect();
}

void USMConnectionSubsystem::OnAuthServiceStateChanged(const EGrpcServiceState ChangedState)
{
	UE_LOG(LogStereoMix, Log, TEXT("gRPC AuthService state changed: %s"), *UEnum::GetValueAsString(ChangedState))
	AuthServiceStateChangedEvent.Broadcast(ChangedState);
}

bool USMConnectionSubsystem::IsAuthenticated() const
{
	return USMGrpcServiceUtil::IsServiceReadyToCall(AuthService) && bIsAuthenticated;
}

void USMConnectionSubsystem::SetTargetEndPoint(EConnectionEndPointTargets Target)
{
	UTurboLinkGrpcConfig* TurboLinkConfig = GetMutableDefault<UTurboLinkGrpcConfig>();

	UE_LOG(LogStereoMix, Verbose, TEXT("SetTargetEndPoint: %s"), *UEnum::GetValueAsString(Target))

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
		TurboLinkConfig->DefaultEndPoint = DevelopmentServerEndPoint;
		break;

	case EConnectionEndPointTargets::Local:
		// 로컬 테스트의 편의를 위해 SecureChannel을 사용하지 않습니다.
		TurboLinkConfig->EnableServerSideTLS = false;
		TurboLinkConfig->ServerRootCerts = TEXT("");
		TurboLinkConfig->DefaultEndPoint = LocalServerEndPoint;
		break;
	}
}
