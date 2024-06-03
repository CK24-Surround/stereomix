// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMFrontendPlayerController.h"
#include "SMPlayerState.h"
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
	UPROPERTY(BlueprintAssignable)
	FCharacterTypeChangedEvent CharacterTypeChangedEvent;
	
	ASMCharacterSelectPlayerController();

	virtual void OnRep_PlayerState() override;

	virtual void BeginPlay() override;

	UFUNCTION(Reliable, Server)
	void ChangeCharacterType(ESMCharacterType NewCharacterType);

protected:
	UFUNCTION()
	void OnCharacterTypeChanged(ESMCharacterType NewCharacterType);
};
