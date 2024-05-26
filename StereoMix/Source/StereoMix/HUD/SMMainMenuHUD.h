// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMBaseHUD.h"
#include "SMMainMenuHUD.generated.h"

/**
 * 
 */
UCLASS()
class STEREOMIX_API ASMMainMenuHUD : public ASMBaseHUD
{
	GENERATED_BODY()

public:
	ASMMainMenuHUD();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:

	UPROPERTY(Transient)
	TObjectPtr<class USMViewModel_MainMenu> MainMenuViewModel;
};
