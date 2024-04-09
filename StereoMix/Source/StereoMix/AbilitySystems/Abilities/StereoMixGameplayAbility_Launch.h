// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "StereoMixGameplayAbility_Launch.generated.h"

class AStereoMixProjectile;
/**
 * 이 GA를 사용하려면 UStereoMixCharacterAttributeSet을 가지고 있어야합니다.
 */
UCLASS()
class STEREOMIX_API UStereoMixGameplayAbility_Launch : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UStereoMixGameplayAbility_Launch();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UFUNCTION()
	void OnFinished();
	
protected:
	UPROPERTY(EditAnywhere, Category = "Projectile")
	TSubclassOf<AStereoMixProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Montage")
	TObjectPtr<UAnimMontage> Montage;

protected:
	UFUNCTION(Server, Reliable)
	void ServerRPCRequestSpawnProjectile(const FVector_NetQuantize10& StartLocation, const FVector_NetQuantize10& CursorLocation);
};
