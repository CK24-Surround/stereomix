//Generated by TurboLink CodeGenerator, do not edit!
#include "GameService_Private.h"

void UGameServiceGetServiceVersionLambdaWrapper::OnResponse(FGrpcContextHandle _Handle, const FGrpcResult& GrpcResult, const FGrpcGameGetServiceVersionResponse& Response)
{
	if (_Handle != this->Handle) return;

	ResponseLambda(GrpcResult, Response);
	InnerClient->OnGetServiceVersionResponse.RemoveDynamic(this, &UGameServiceGetServiceVersionLambdaWrapper::OnResponse);
}

