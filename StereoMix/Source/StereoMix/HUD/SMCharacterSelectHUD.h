// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMBaseHUD.h"
#include "SMCharacterSelectHUD.generated.h"

class USMCharacterSelectionViewModel;
class ASMCharacterSelectState;
/**
 * 
 */
UCLASS()
class STEREOMIX_API ASMCharacterSelectHUD : public ASMBaseHUD
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<ASMCharacterSelectState> CharacterSelectState;

	UPROPERTY()
	TObjectPtr<USMCharacterSelectionViewModel> CharacterSelectViewModel;

public:
	ASMCharacterSelectHUD();

	virtual void BeginPlay() override;
};
