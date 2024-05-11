// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMCatchableItem.h"
#include "SMCatchableItem_AttributeChanger.generated.h"

class USMTeamComponent;
class ASMTile;

UCLASS()
class STEREOMIX_API ASMCatchableItem_AttributeChanger : public ASMCatchableItem
{
	GENERATED_BODY()

public:
	ASMCatchableItem_AttributeChanger();

public:
	virtual USMCatchInteractionComponent* GetCatchInteractionComponent() override;

public:
	virtual void ActivateItem(ESMTeam SourceTeam) override;

protected:
	void TriggerCountTimerStart();

	void TriggerCountTimerCallback();

	/** 버프가 적용될때마다 호출되야합니다. */
	void HandleApplyItem();

	/** 아이템을 적용해야할 범위 내에 있는 액터들을 찾아내서 반환합니다. */
	TArray<TWeakObjectPtr<AActor>> ScanActorsToApplyItem();

	/** 아이템을 적용하기에 유효한 타겟인지 검증합니다. */
	bool IsValidActorToApplyItem(AActor* TargetActor);

protected:
	UPROPERTY(VisibleAnywhere, Category = "CIC")
	TObjectPtr<USMCatchInteractionComponent_CatchableItem_AttributeChanger> CIC;

	UPROPERTY(VisibleAnywhere, Category = "TeamComponent")
	TObjectPtr<USMTeamComponent> TeamComponent;

public:
	struct FTriggerData
	{
		FTimerHandle TimerHandle;
		int32 CurrentTriggerCount = 0;
	};

public:
	TArray<TWeakObjectPtr<ASMTile>> TriggeredTiles;

protected:
	UPROPERTY(EditAnywhere, Category = "Item", DisplayName = "총량")
	float TotalAmount = 50.0f;

	UPROPERTY(EditAnywhere, Category = "Item", DisplayName = "지속시간")
	float Duration = 5.0f;

	UPROPERTY(EditAnywhere, Category = "Item", DisplayName = "주기")
	float Interval = 0.5f;

	int32 TriggerCount = 0;

	float Amount = 0;

	FTriggerData TriggerData;
};
