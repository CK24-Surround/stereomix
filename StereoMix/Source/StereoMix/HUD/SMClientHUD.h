// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "SMClientHUD.generated.h"

/**
 * 
 */
UCLASS()
class STEREOMIX_API ASMClientHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void InitPlayerState(APlayerState* PlayerState);

	APlayerState* GetOwningPlayerState() const { return OwningPlayerState.Get(); }

private:
	TWeakObjectPtr<APlayerState> OwningPlayerState;
};
