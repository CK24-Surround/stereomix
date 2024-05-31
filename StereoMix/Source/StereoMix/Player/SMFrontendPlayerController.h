// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SMFrontendPlayerController.generated.h"

class USMGuestLoginViewModel;
/**
 * 
 */
UCLASS()
class STEREOMIX_API ASMFrontendPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ASMFrontendPlayerController();

	virtual void BeginPlay() override;
};
