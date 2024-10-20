//Generated by TurboLink CodeGenerator, do not edit!
#pragma once
#include "TurboLinkGrpcClient.h"
#include "SGame/GameMessage.h"
#include "GameClient.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnGameServiceGetServiceVersionResponse, FGrpcContextHandle, Handle, const FGrpcResult&, GrpcResult, const FGrpcGameGetServiceVersionResponse&, Response);

UCLASS(ClassGroup = TurboLink, BlueprintType)
class TURBOLINKGRPC_API UGameServiceClient : public UGrpcClient
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnGameServiceGetServiceVersionResponse OnGetServiceVersionResponse;

public:
	UFUNCTION(BlueprintCallable, Category = TurboLink)
	FGrpcContextHandle InitGetServiceVersion();

	UFUNCTION(BlueprintCallable, Category = TurboLink, meta = (AdvancedDisplay = 2))
	void GetServiceVersion(FGrpcContextHandle Handle, const FGrpcGameGetServiceVersionRequest& Request, FGrpcMetaData MetaData = FGrpcMetaData(), float DeadLineSeconds = 0.f);

public:
	virtual void Shutdown() override;

	UFUNCTION(BlueprintCallable, Category = TurboLink)
	void TryCancel(FGrpcContextHandle Handle);
};

