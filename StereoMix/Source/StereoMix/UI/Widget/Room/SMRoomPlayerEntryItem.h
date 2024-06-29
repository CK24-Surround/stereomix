// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Games/Room/SMRoomPlayerState.h"
#include "UObject/Object.h"

#include "SMRoomPlayerEntryItem.generated.h"

/**
 *
 */
UCLASS()
class STEREOMIX_API USMRoomPlayerEntryItem : public UObject
{
	GENERATED_BODY()

public:
	void Init(ASMRoomPlayerState* PlayerState);

	ASMRoomPlayerState* GetOwningPlayerState() const { return OwningPlayerState.Get(); }

private:
	TWeakObjectPtr<ASMRoomPlayerState> OwningPlayerState;
};
