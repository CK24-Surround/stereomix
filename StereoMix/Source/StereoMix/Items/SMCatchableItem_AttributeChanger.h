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
	virtual void ActivateItem(AActor* Activator) override;

protected:
	/** 반복 적용을 위한 타이머를 시작합니다. */
	void TriggerCountTimerStart();

	/** 타이머가 종료될 때 마다 실행됩니다. 이 때 타이머를 재시작해 카운트를 만큼 반복되도록 처리합니다. */
	void TriggerCountTimerCallback();

	/** 버프가 적용될때마다 호출되야합니다. */
	void ApplyItem();

	/** 트레이스를 통해 트리거된 타일 위에 있는 액터들을 반환합니다. */
	TArray<TWeakObjectPtr<AActor>> GetActorsOnTriggeredTiles(ECollisionChannel TraceChannel);

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
