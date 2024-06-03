// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMPlayerState.h"
#include "SMCharacterSelectPlayerState.generated.h"

class ASMCharacterSelectState;
/**
 * 
 */
UCLASS()
class STEREOMIX_API ASMCharacterSelectPlayerState : public ASMPlayerState
{
	GENERATED_BODY()

	TWeakObjectPtr<ASMCharacterSelectState> CharacterSelectState;
	
public:
	ASMCharacterSelectPlayerState();
	
	virtual void BeginPlay() override;

	virtual bool CanChangeTeam(ESMTeam NewTeam) const override;

	virtual bool CanChangeCharacterType(ESMCharacterType NewCharacterType) const override;

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void ChangeCharacterType(ESMCharacterType NewCharacterType);
};
