// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TurboLinkGrpcService.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SMGrpcServiceUtil.generated.h"

struct FGrpcContextHandle;
class UGrpcService;
/**
 * 
 */
UCLASS()
class STEREOMIX_API USMGrpcServiceUtil : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "SMGrpcServiceUtilFunctions")
	static bool IsServiceReadyToCall(const UGrpcService* Service);

	UFUNCTION(BlueprintCallable, Category = "SMGrpcServiceUtilFunctions")
	static bool IsBusy(const UGrpcClient* Client, const FGrpcContextHandle& Handle);
};
