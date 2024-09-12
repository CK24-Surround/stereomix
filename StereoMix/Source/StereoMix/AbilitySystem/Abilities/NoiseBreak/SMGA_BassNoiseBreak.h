// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMGA_NoiseBreak.h"
#include "Data/SMTeam.h"
#include "SMGA_BassNoiseBreak.generated.h"

class UAnimMontage;
class UGameplayEffect;

/**
 * 
 */
UCLASS(Abstract)
class STEREOMIX_API USMGA_BassNoiseBreak : public USMGA_NoiseBreak
{
	GENERATED_BODY()

public:
	USMGA_BassNoiseBreak();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	/** 타겟데이터를 수신 받으면 호출됩니다. */
	void OnReceiveTargetData(const FGameplayAbilityTargetDataHandle& GameplayAbilityTargetDataHandle, FGameplayTag GameplayTag);

	/** 매치기의 핵심 로직입니다. 착지 시 호출됩니다. */
	UFUNCTION()
	void OnNoiseBreak(ASMPlayerCharacterBase* LandedCharacter);

	/** 스매시 종료 애니메이션에 진입 시 호출됩니다. 사실상 스매시 GA가 종료됩니다. */
	UFUNCTION()
	void OnNoiseBreakEnd(FGameplayEventData Payload);

	/** 사거리 내를 타게팅 했다면 InTargetLocation이 그대로, 사거리 밖을 타게팅 했다면 사거리에 맞게 조정된 InTargetLocation이 반환됩니다. */
	FVector CalculateMaxDistanceLocation(const FVector& InStartLocation, const FVector& InTargetLocation);

	/** 서버로 시전위치와 타겟 위치를 전송합니다. */
	void SendToServerLocationData(const FVector& InStartLocation, const FVector& InTargetLocation);

	/** 정점 높이를 기준으로 타겟을 향하는 벨로시티를 통해 캐릭터를 도약 시킵니다. */
	void LaunchCharacterToTargetWithApex(const FVector& InStartLocation, const FVector& InTargetLocation, float InGravityZ);

	/** 타일을 점령합니다. */
	void TileCapture();

	UPROPERTY(EditAnywhere, Category = "Design|Montage")
	TMap<ESMTeam, TObjectPtr<UAnimMontage>> SmashMontage;

	UPROPERTY()
	TObjectPtr<UAnimMontage> CachedNoiseBreakMontage;

	UPROPERTY(EditAnywhere, Category = "Design|GAS|GE")
	TSubclassOf<UGameplayEffect> DamageGE;

	UPROPERTY(EditAnywhere, Category = "Design|Jump", DisplayName = "매치기 정점 높이")
	float SmashApexHeight = 500.0f;

	UPROPERTY(EditAnywhere, Category = "Design|Jump", DisplayName = "매치기 시 적용되는 중력 스케일")
	float NoiseBreakGravityScale = 3.0f;

	float OriginalGravityScale = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Design")
	float TotalTriggerTime = 0.267f;

	FName OriginalCollisionProfileName;
};
