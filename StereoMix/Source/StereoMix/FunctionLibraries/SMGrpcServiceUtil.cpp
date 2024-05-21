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
	ensure(Client);
	return Client->GetContextState(Handle) == EGrpcContextState::Busy;
}
