// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SMGameplayAbility.h"
#include "Utilities/SMTeam.h"
#include "SMGameplayAbility_Smash.generated.h"

class UNiagaraSystem;
class ASMTile;
/**
 * 24.04.13 수정
 * 잡고 있는 상태에서만 활성화 가능하며 잡고 있는 대상을 매치는 어빌리티입니다.
 * 대상을 매치며 해당 위치의 타일을 트리거합니다. 매쳐지면 대상은 디태치됩니다.
 *
 * Smashed 태그가 제거되는 시점에 타겟측에 Smashed GA가 활성화됩니다. 
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
	void OnReceiveTargetData(const FGameplayAbilityTargetDataHandle& GameplayAbilityTargetDataHandle, FGameplayTag GameplayTag);
	
	UFUNCTION()
	void OnSmash();

	UFUNCTION()
	void OnCompleted();

protected:
	void ReleaseCatch(ASMPlayerCharacter* TargetCharacter);

protected:
	// 타일을 트리거합니다. 서버에서 호출되어야합니다.
	void TileTrigger(ASMPlayerCharacter* InTargetCharacter);

	void ProcessContinuousTileTrigger();

protected:
	UPROPERTY(EditAnywhere, Category = "Montage")
	TObjectPtr<UAnimMontage> SmashMontage;

	UPROPERTY(EditAnywhere, Category = "FX")
	TMap<ESMTeam, TObjectPtr<UNiagaraSystem>> OnSmashFX;

protected:
	UPROPERTY(EditAnywhere, Category = "Smash")
	float ApexHeight = 500.0f;

	UPROPERTY(EditAnywhere, Category = "Smash")
	float SmashGravityScale = 3.0f;
	
	float OriginalGravityScale = 0.0f;

protected:
	struct FTileTriggerData
	{
		int32 TriggerCount = 0;
		int32 MaxTriggerCount = 3;
		// TotalTriggerTime는 총 몇 초에 걸쳐 타일 트리거가 마무리 되어야할지를 나타냅니다.
		float TotalTriggerTime = 0.2f;
		FVector TriggerStartLocation;
		ESMTeam SourceTeam = ESMTeam::None;
		float TileHorizonSize = 0.0f;
		float Range = 0.0f;
	};

	FTileTriggerData TileTriggerData;
};
