// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SMGameplayAbility.h"
#include "Utilities/SMTeam.h"
#include "SMGameplayAbility_Smash.generated.h"

class UNiagaraSystem;
class ASMTile;
/**
 * 24.04.24 수정
 * 잡고 있는 상태에서만 활성화 가능하며 잡고 있는 대상을 매치는 어빌리티입니다.
 * 대상을 매치며 해당 위치의 타일을 트리거합니다. 매쳐지면 대상은 디태치됩니다.
 *
 * Smashed 태그가 제거되는 시점에 타겟측에 Smashed GA가 활성화됩니다.
 * 타일 트리거에는 타일 트리거 GA가 사용됩니다.
 *
 * 디태치에서 처리되는 목록입니다.
 *     1. 타겟의 콜리전 활성화
 *     2. 움직임 활성화 (여기서 움직임은 조작에 의한 움직임이 아닌 중력 등에 의한 무브먼트 동작을 말합니다.
 *     3. 시전자와 동일한 방향을 바라보도록 피격자의 회전을 리셋해줍니다.
 *     4. 클라이언트 서버 움직임 보정 활성화
 *     5. 피격자 측 카메라 뷰 복귀
 *
 * 성공적으로 어빌리티가 종료되면 다음과 같은 태그 상태를 갖게 됩니다.
 * 시전 측 제거 태그: 잡기 상태 태그 
 * 피격 측 제거 태그: 잡힌 상태 태그
 * 
 */
UCLASS()
class STEREOMIX_API USMGameplayAbility_Smash : public USMGameplayAbility
{
	GENERATED_BODY()

public:
	USMGameplayAbility_Smash();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	/** 애님 노티파이가 트리거될때 호출됩니다. */
	void OnReceiveTargetData(const FGameplayAbilityTargetDataHandle& GameplayAbilityTargetDataHandle, FGameplayTag GameplayTag);

	/** 매치기의 핵심 로직입니다. */
	UFUNCTION()
	void OnSmash();

	UFUNCTION()
	void OnCompleted();

protected:
	/** 잡은 상대를 풀어줍니다. */
	void ReleaseCatch(ASMPlayerCharacter* TargetCharacter);

protected:
	// 타일을 트리거합니다. 서버에서 호출되어야합니다.
	void TileTrigger(ASMPlayerCharacter* InTargetCharacter);

	/** 스플래시 대미지를 적용합니다. */
	void ApplySmashSplashDamage(const FVector& TileLocation, float TileHorizonSize);

protected:
	UPROPERTY(EditAnywhere, Category = "Montage")
	TObjectPtr<UAnimMontage> SmashMontage;

	UPROPERTY(EditAnywhere, Category = "FX")
	TMap<ESMTeam, TObjectPtr<UNiagaraSystem>> OnSmashFX;

	UPROPERTY(EditAnywhere, Category = "FX")
	TMap<ESMTeam, TObjectPtr<UNiagaraSystem>> SplashDamageFX;

	UPROPERTY(EditAnywhere, Category = "GAS|GE")
	TSubclassOf<UGameplayEffect> DamageGE;

protected:
	UPROPERTY(EditAnywhere, Category = "Smash", DisplayName = "매치기 대미지")
	float SmashDamage = 40.0f;

	UPROPERTY(EditAnywhere, Category = "Smash|Jump", DisplayName = "매치기 도약 거리(타일 기준)")
	int32 SmashRangeByTile = 6;

	UPROPERTY(EditAnywhere, Category = "Smash|Jump", DisplayName = "매치기 정점 높이")
	float ApexHeight = 500.0f;

	UPROPERTY(EditAnywhere, Category = "Smash|Jump", DisplayName = "매치기 시 적용되는 중력 스케일")
	float SmashGravityScale = 3.0f;

	float OriginalGravityScale = 0.0f;

protected:
	UPROPERTY(EditAnywhere, Category = "Smash|TileTirgger", DisplayName = "매치기 1회에 타일이 트리거 되는 횟수")
	int32 MaxTriggerCount = 3;
};
