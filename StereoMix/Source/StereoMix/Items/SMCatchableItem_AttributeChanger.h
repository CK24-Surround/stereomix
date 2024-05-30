// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMCatchableItem.h"
#include "Data/SMLocalTeam.h"
#include "SMCatchableItem_AttributeChanger.generated.h"

class UFMODEvent;
class UNiagaraSystem;
class UGameplayEffect;
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
	virtual void ActivateItem(AActor* InActivator) override;

protected:
	/** 활성자에게 GE를 적용합니다. */
	void ApplyGEToActivator();

	/** 반복 적용을 위한 타이머를 시작합니다. */
	void TriggerCountTimerStart();

	/** 타이머가 종료될 때 마다 실행됩니다. 이 때 타이머를 재시작해 카운트를 만큼 반복되도록 처리합니다. */
	void TriggerCountTimerCallback();

	/** 아이템 적용 시작 시 호출됩니다. */
	void ApplyItemByStart(TArray<AActor*> ActorsToApply);

	/** 아이템 적용 중 일정 주기로 지속적으로 호출됩니다. */
	void ApplyItemByWhile(TArray<AActor*> ActorsToApply);

	/** 아이템 효과를 적용합니다. 효과가 적용될때마다 호출되야합니다. */
	/** 아이템 효과를 적용할 수 있는 액터들을 반환합니다. */
	TArray<AActor*> GetConfirmedActorsToApplyItem();

	/** 트레이스를 통해 트리거된 타일 위에 있는 액터들을 반환합니다. */
	TArray<AActor*> GetActorsOnTriggeredTiles(ECollisionChannel TraceChannel);

	/** 아이템을 적용하기에 유효한 타겟인지 검증합니다. */
	bool IsValidActorToApplyItem(AActor* TargetActor);

	/** FX를 재생합니다. */
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCPlayActivateHealItemFX(AActor* InActivator, const TArray<TWeakObjectPtr<ASMTile>>& InTrigeredTiles);

	/** 로컬에서 플레이하고 있는 팀과 시전자의 팀이 같은지 확인합니다. 추후 펑션 라이브러리 클래스로 옮길 예정입니다. */
	bool IsSameTeamWithLocalTeam(AActor* TargetActor) const;

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
	/** 아이템 활성자입니다. */
	TWeakObjectPtr<AActor> Activator;

	/** 트리거된 타일을 저장하고 있습니다. CIC에서 저장됩니다. */
	TArray<TWeakObjectPtr<ASMTile>> TriggeredTiles;

	/** 현재 트리거 횟수입니다. */
	int32 TriggerCount = 0;

	/** 활성자의 팀원에게 적용할 트리거 당 적용량입니다. */
	float TeamAmount = 0;

	FTriggerData TriggerData;

protected:
	UPROPERTY(EditAnywhere, Category = "Design|FX")
	TMap<ESMLocalTeam, TObjectPtr<UNiagaraSystem>> ActivateEffect;

	UPROPERTY(EditAnywhere, Category = "Design|FX")
	TMap<ESMLocalTeam, TObjectPtr<UFMODEvent>> ActivateSound;

	UPROPERTY(EditAnywhere, Category = "Design", DisplayName = "지속시간")
	float Duration = 5.0f;

	UPROPERTY(EditAnywhere, Category = "Design", DisplayName = "주기")
	float Interval = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Design")
	uint32 bDrawDebug:1 = false;

protected:
	UPROPERTY(EditAnywhere, Category = "Design|Activator", DisplayName = "즉시 적용 GE")
	TSubclassOf<UGameplayEffect> ActivatorInstantGE;

	UPROPERTY(EditAnywhere, Category = "Design|Activator", DisplayName = "지속 적용 GE")
	TSubclassOf<UGameplayEffect> ActivatorDurationGE;

	UPROPERTY(EditAnywhere, Category = "Design|Activator", DisplayName = "즉시 적용량")
	float ActivatorInstantAmount = 30.0f;

	UPROPERTY(EditAnywhere, Category = "Design|Activator", DisplayName = "지속 적용 총량")
	float ActivatorTotalAmount = 30.0f;

	UPROPERTY(EditAnywhere, Category = "Design|Activator", DisplayName = "지속시간")
	float ActivatorDuration = 5.0f;

	UPROPERTY(EditAnywhere, Category = "Design|Activator", DisplayName = "주기")
	float ActivatorInterval = 0.5f;

protected:
	UPROPERTY(EditAnywhere, Category = "Design|Team", DisplayName = "즉시 적용 GE")
	TSubclassOf<UGameplayEffect> TeamInstantGE;

	UPROPERTY(EditAnywhere, Category = "Design|Team", DisplayName = "지속 적용 GE")
	TSubclassOf<UGameplayEffect> TeamDurationGE;

	UPROPERTY(EditAnywhere, Category = "Design|Team", DisplayName = "즉시 적용량")
	float TeamInstantAmount = 20.0f;

	UPROPERTY(EditAnywhere, Category = "Design|Team", DisplayName = "지속 적용 총량")
	float TeamTotalAmount = 30.0f;
};
