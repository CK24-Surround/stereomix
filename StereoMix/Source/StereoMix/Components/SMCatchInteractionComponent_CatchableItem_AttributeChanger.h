// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMCatchInteractionComponent_CatchableItem.h"
#include "Tiles/SMTile.h"
#include "SMCatchInteractionComponent_CatchableItem_AttributeChanger.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STEREOMIX_API USMCatchInteractionComponent_CatchableItem_AttributeChanger : public USMCatchInteractionComponent_CatchableItem
{
	GENERATED_BODY()

public:
	USMCatchInteractionComponent_CatchableItem_AttributeChanger();

public:
	virtual void OnSpecialActionPerformed(AActor* Instigator, ESpecialAction InSpecialAction) override;

	virtual void OnSpecialActionEnded(AActor* Instigator, ESpecialAction InSpecialAction, float InMagnitude, TSubclassOf<UGameplayEffect> DamageGE, float DamageAmount) override;

protected:
	void InternalOnSmashedEnded(AActor* Instigator, float InMagnitude);

	/** 현재 아이템 위치 아래의 타일을 트리거하고 이 타일을 반환합니다. */
	ASMTile* TileTrigger();

	/** 스매시에 의해 트리거 된 타일들을 오너 액터(아이템)에 저장합니다. */
	void SaveTriggeredTilesBySmash(ASMTile* InTriggeredTile, float InMagnitude);
};
