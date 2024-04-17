// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SMGameplayAbility.h"
#include "Utilities/SMTeam.h"
#include "SMGameplayAbility_Launch.generated.h"

class UNiagaraSystem;
class ASMProjectile;

/**
 * 클라이언트의 마우스 위치를 서버로 전송해 이 데이터를 기반으로 서버에서 투사체를 생성하는 어빌리티입니다.
 */
UCLASS()
class STEREOMIX_API USMGameplayAbility_Launch : public USMGameplayAbility
{
	GENERATED_BODY()

public:
	USMGameplayAbility_Launch();

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

protected:
	UPROPERTY(EditAnywhere, Category = "FX")
	TMap<ESMTeam, TObjectPtr<UNiagaraSystem>> LaunchFX;
};
