// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "SMHoldableItemBase.h"
#include "Data/SMLocalTeam.h"
#include "SMHoldableItem_ChangeAttribute.generated.h"

class UNiagaraSystem;
class UGameplayEffect;

UCLASS()
class STEREOMIX_API ASMHoldableItem_ChangeAttribute : public ASMHoldableItemBase
{
	GENERATED_BODY()

public:
	ASMHoldableItem_ChangeAttribute(const FObjectInitializer& ObjectInitializer);

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void ActivateItemByNoiseBreak(AActor* InActivator, const TArray<TWeakObjectPtr<ASMTile>>& TilesToBeCaptured) override;

	bool GetIsActivated() const { return bIsActivated; }

	void SetIsActorEnabled(bool bNewIsActorEnabled);

protected:
	void TriggerCountTimerCallback();

	TArray<AActor*> GetConfirmedActorsToApplyItem();

	TArray<AActor*> GetActorsOnTriggeredTiles(ECollisionChannel TraceChannel);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayActivateTileFX(AActor* InActivator, const TArray<TWeakObjectPtr<ASMTile>>& InTriggeredTiles);

	UFUNCTION()
	void OnRep_bIsActorHidden();

	UPROPERTY(EditAnywhere, Category = "Design")
	TMap<ESMLocalTeam, TObjectPtr<UNiagaraSystem>> ActivateVFX;

	UPROPERTY(EditAnywhere, Category = "Design")
	TSubclassOf<UGameplayEffect> GEForActivator;

	UPROPERTY(EditAnywhere, Category = "Design")
	FGameplayTag DataTagForActivator;

	UPROPERTY(EditAnywhere, Category = "Design")
	float MagnitudeForActivator = 10.0f;

	UPROPERTY(EditAnywhere, Category = "Design")
	TSubclassOf<UGameplayEffect> GE;

	UPROPERTY(EditAnywhere, Category = "Design")
	FGameplayTag DataTag;

	UPROPERTY(EditAnywhere, Category = "Design")
	ESMLocalTeam TeamTypeToApply = ESMLocalTeam::Equal;

	UPROPERTY(EditAnywhere, Category = "Design")
	float TotalMagnitude = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Design", meta = (ClampMin = "0.0"))
	float Duration = 5.0f;

	UPROPERTY(EditAnywhere, Category = "Design", meta = (ClampMin = "1"))
	int32 TriggerCount = 1;

	TArray<TWeakObjectPtr<ASMTile>> CachedTriggeredTiles;

	uint8 bIsActivated:1 = false;

	UPROPERTY(ReplicatedUsing = "OnRep_bIsActorHidden")
	uint32 bIsActorEnabled:1 = true;
};
