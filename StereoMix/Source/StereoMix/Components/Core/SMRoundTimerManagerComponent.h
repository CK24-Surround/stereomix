// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SMRoundTimerManagerComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRoundTimerTimeExpiredDelegate);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRoundTimerRemainingTimeDelegate, int32, RemainTime);

UENUM(BlueprintType)
enum class ESMTimerState : uint8
{
	PreRound,
	InRound,
	PostRound
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STEREOMIX_API USMRoundTimerManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USMRoundTimerManagerComponent();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable)
	void StartTimer();

	ESMTimerState GetTimerState() const { return TimerState; }

	int32 GetRemainingTime() const { return RemainingTime; }

	UPROPERTY(BlueprintAssignable)
	FRoundTimerRemainingTimeDelegate OnRemainingRoundTimeChanged;

	UPROPERTY(BlueprintAssignable)
	FRoundTimerTimeExpiredDelegate OnPreRoundTimeExpired;

	UPROPERTY(BlueprintAssignable)
	FRoundTimerTimeExpiredDelegate OnRoundTimeExpired;

	UPROPERTY(BlueprintAssignable)
	FRoundTimerTimeExpiredDelegate OnPostRoundTimeExpired;

protected:
	void CountdownTime();

	void SetRemainingTime(int32 NewTime);

	UFUNCTION()
	void OnRep_RemainingTime();

	UPROPERTY(Replicated)
	ESMTimerState TimerState = ESMTimerState::PreRound;

	UPROPERTY(EditAnywhere, Category = "Design", meta = (ClampMin = "0"))
	int32 PreRoundTime = 15;

	UPROPERTY(EditAnywhere, Category = "Design", meta = (ClampMin = "0"))
	int32 RoundTime = 150;

	UPROPERTY(EditAnywhere, Category = "Design", meta = (ClampMin = "0"))
	int32 PostRoundTime = 10;

	UPROPERTY(ReplicatedUsing = "OnRep_RemainingTime")
	int32 RemainingTime = 0;

	FTimerHandle CountdownTimerHandle;
};
