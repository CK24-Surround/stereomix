// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMGrpcClientSubsystem.h"
#include "SGame/GameService.h"
#include "SMGrpcGameSubsystem.generated.h"

class UGameService;

/**
 * 
 */
UCLASS()
class STEREOMIX_API USMGrpcGameSubsystem : public USMGrpcClientSubsystem
{
	GENERATED_BODY()

public:
	USMGrpcGameSubsystem();

	UGameService* GetGameService() const { return Cast<UGameService>(GrpcService); }
};
