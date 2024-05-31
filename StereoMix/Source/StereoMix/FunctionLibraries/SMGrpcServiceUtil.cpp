// Copyright Surround, Inc. All Rights Reserved.


#include "SMGrpcServiceUtil.h"

#include "TurboLinkGrpcClient.h"
#include "TurboLinkGrpcService.h"

bool USMGrpcServiceUtil::IsServiceReadyToCall(const UGrpcService* Service)
{
	if (Service)
	{
		const EGrpcServiceState State = Service->GetServiceState();
		return State == EGrpcServiceState::Idle || State == EGrpcServiceState::Ready;
	}
	return false;
}

bool USMGrpcServiceUtil::IsBusy(const UGrpcClient* Client, const FGrpcContextHandle& Handle)
{
	if (!ensureMsgf(Client, TEXT("GrpcClient is nullptr.")))
	{
		return true;
	}
	return Client->GetContextState(Handle) == EGrpcContextState::Busy;
}
