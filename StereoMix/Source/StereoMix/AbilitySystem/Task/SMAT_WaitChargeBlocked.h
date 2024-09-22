// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "SMAT_WaitChargeBlocked.generated.h"

DECLARE_DELEGATE_OneParam(FOnChargeBlockingDelegate, AActor* /*TargetActor*/);

class ASMBassCharacter;
/**
 * 
 */
UCLASS()
class STEREOMIX_API USMAT_WaitChargeBlocked : public UAbilityTask
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static USMAT_WaitChargeBlocked* WaitChargeBlocked(UGameplayAbility* OwningAbility, const FGameplayTagContainer& IgnoreTags);

	FOnChargeBlockingDelegate OnChargeBlocked;

protected:
	virtual void Activate() override;

	virtual void OnDestroy(bool bInOwnerFinished) override;

	UFUNCTION()
	void OnChargeOverlappedCallback(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnChargeBlockedCallback(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	TWeakObjectPtr<ASMBassCharacter> SourceCharacter;

	FGameplayTagContainer ChargeIgnoreTags;
};
