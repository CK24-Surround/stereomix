// Copyright Surround, Inc. All Rights Reserved.


#include "SMGrpcClientSubsystem.h"

#include "StereoMix.h"
#include "TurboLinkGrpcManager.h"

USMGrpcClientSubsystem::USMGrpcClientSubsystem()
{
}

USMGrpcClientSubsystem::USMGrpcClientSubsystem(const FString& InServiceName) : ServiceName(InServiceName)
{
}

void USMGrpcClientSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	GrpcSubsystem = Collection.InitializeDependency<USMGrpcSubsystem>();
	GrpcManager = GrpcSubsystem->GetGrpcManager();
	GrpcService = GrpcManager->MakeService(ServiceName);

	GrpcService->OnServiceStateChanged.AddDynamic(this, &USMGrpcClientSubsystem::OnServiceStateChanged);
}

void USMGrpcClientSubsystem::Connect() const
{
	if (GrpcService)
	{
		GrpcService->Connect();
	}
}

void USMGrpcClientSubsystem::OnServiceStateChanged(const EGrpcServiceState ServiceState)
{
	UE_LOG(LogStereoMix, Verbose, TEXT("[SMGrpcClientSubsystem] Service '%s' State Changed: %s"), *ServiceName, *UEnum::GetValueAsString(ServiceState))
}

UGrpcService* USMGrpcClientSubsystem::GetGrpcService() const
{
	return GrpcService;
}
