// Copyright Surround, Inc. All Rights Reserved.


#include "SMGrpcClientSubsystem.h"

#include "StereoMix.h"
#include "StereoMixLog.h"
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

	GrpcService->OnServiceStateChanged.AddUniqueDynamic(this, &USMGrpcClientSubsystem::HandleServiceStateChanged);
}

void USMGrpcClientSubsystem::Deinitialize()
{
	Super::Deinitialize();

	GrpcService->OnServiceStateChanged.RemoveDynamic(this, &USMGrpcClientSubsystem::HandleServiceStateChanged);
}

void USMGrpcClientSubsystem::Connect() const
{
	if (GrpcService)
	{
		GrpcService->Connect();
	}
}

void USMGrpcClientSubsystem::HandleServiceStateChanged(const EGrpcServiceState ServiceState)
{
	UE_LOG(LogStereoMix, Verbose, TEXT("[SMGrpcClientSubsystem] Service '%s' State Changed: %s"), *ServiceName, *UEnum::GetValueAsString(ServiceState))
	if (OnServiceStateChanged.IsBound())
	{
		OnServiceStateChanged.Broadcast(ServiceState);
	}
}

UGrpcService* USMGrpcClientSubsystem::GetGrpcService() const
{
	return GrpcService;
}
