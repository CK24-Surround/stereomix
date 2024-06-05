// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Games/CharacterSelect/SMCharacterSelectPlayerState.h"
#include "UObject/Object.h"
#include "SMCharacterSelectPlayerEntryItem.generated.h"

/**
 * 
 */
UCLASS()
class STEREOMIX_API USMCharacterSelectPlayerEntryItem : public UObject
{
	GENERATED_BODY()

public:
	void Init(ASMCharacterSelectPlayerState* PlayerState);

	ASMCharacterSelectPlayerState* GetOwningPlayerState() const { return OwningPlayerState.Get(); }

private:
	TWeakObjectPtr<ASMCharacterSelectPlayerState> OwningPlayerState;
};
