// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SMRoundTimerManagerComponent.generated.h"

class USMTileManagerComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRoundTimerTimeExpiredDelegate);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRoundTimerRemainingTimeDelegate, int32, RemainTime);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STEREOMIX_API USMRoundTimerManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USMRoundTimerManagerComponent();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void InitializeComponent() override;

	UFUNCTION(BlueprintCallable)
	void StartTimer();

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

	UPROPERTY(EditAnywhere, Category = "Design")
	uint32 AutoStartTimer:1 = true;

	UPROPERTY(EditAnywhere, Category = "Design")
	uint32 AutoLeaveLevel:1 = true;

	UPROPERTY(EditAnywhere, Category = "Design", meta = (ClampMin = "0"))
	int32 PreRoundTime = 15;

	UPROPERTY(EditAnywhere, Category = "Design", meta = (ClampMin = "0"))
	int32 RoundTime = 150;

	UPROPERTY(EditAnywhere, Category = "Design", meta = (ClampMin = "0"))
	int32 PostRoundTime = 60;

	UPROPERTY(ReplicatedUsing = "OnRep_RemainingTime")
	int32 RemainingTime = 0;

	FTimerHandle CountdownTimerHandle;
};
