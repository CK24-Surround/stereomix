// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StereoMixGameplayAbility.h"
#include "StereoMixGameplayAbility_Launch.generated.h"

class ASMProjectile;
/**
 * 이 GA를 사용하려면 UStereoMixCharacterAttributeSet을 가지고 있어야합니다.
 */
UCLASS()
class STEREOMIX_API UStereoMixGameplayAbility_Launch : public UStereoMixGameplayAbility
{
	GENERATED_BODY()

public:
	UStereoMixGameplayAbility_Launch();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
protected:
	UPROPERTY(EditAnywhere, Category = "Projectile")
	TSubclassOf<ASMProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Montage")
	TObjectPtr<UAnimMontage> Montage;

protected:
	/** 서버에 투사체 생성 및 발사를 요청합니다. */
	UFUNCTION(Server, Reliable)
	void ServerRPCRequestProjectile(const FVector_NetQuantize10& StartLocation, const FVector_NetQuantize10& CursorLocation);
};
