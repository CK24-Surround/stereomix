// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CountdownTimerComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTCountdownimerDelegate);

UCLASS(ClassGroup=(Game), meta=(BlueprintSpawnableComponent))
class STEREOMIX_API UCountdownTimerComponent : public UActorComponent
{
	GENERATED_BODY()

	FTimerHandle TimerHandle;

	// TODO: float으로 바꾸고 tick interval을 변경 가능하도록 수정하기
	
	UPROPERTY(ReplicatedUsing=OnRep_RemainingTime)
	int32 RemainingTime;

	UFUNCTION()
	void OnRep_RemainingTime() const;
	
public:
	FTCountdownimerDelegate OnCountdownTick;
	FTCountdownimerDelegate OnCountdownFinished;
	FTCountdownimerDelegate OnCountdownCancelled;
	
	UCountdownTimerComponent();

	int32 GetRemainingTime() const { return RemainingTime; }

protected:
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PerformCountdownTick();

public:
	void StartCountdown(int32 Seconds);

	void CancelCountdown();
};
