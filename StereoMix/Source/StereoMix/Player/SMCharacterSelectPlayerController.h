// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMFrontendPlayerController.h"
#include "Games/SMCharacterSelectState.h"
#include "SMCharacterSelectPlayerController.generated.h"

class ASMCharacterSelectPlayerState;
/**
 * 
 */
UCLASS()
class STEREOMIX_API ASMCharacterSelectPlayerController : public ASMFrontendPlayerController
{
	GENERATED_BODY()

	TWeakObjectPtr<ASMCharacterSelectPlayerState> CharacterSelectPlayerState;
	
public:
	ASMCharacterSelectPlayerController();

	virtual void InitPlayerState() override;

	virtual void BeginPlay() override;

protected:
	UFUNCTION()
	void OnCharacterTypeChanged(ESMCharacterType NewCharacterType);
};
