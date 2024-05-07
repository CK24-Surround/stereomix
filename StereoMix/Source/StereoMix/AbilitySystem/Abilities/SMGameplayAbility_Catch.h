// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMGameplayAbility.h"
#include "Characters/SMPlayerCharacter.h"
#include "SMGameplayAbility_Catch.generated.h"

class UNiagaraSystem;

/**
 * 24.04.13 수정
 * 클라이언트에서 마우스 좌표 정보를 받아 서버로 전송한 후 서버에서 해당 정보를 기반으로 잡기를 수행합니다.
 * 필터링을 통해 잡을 캐릭터를 선별하고 해당 캐릭터를 잡는 로직을 수행하는 어빌리티입니다.
 * 최종적으로 피격자는 Caught 어빌리티를 활성화하게 됩니다.
 * 
 * 잡는 로직은 시전자에게 타겟이 어태치되고 타겟의 카메라가 시전자의 카메라로 전환됩니다. 여기서 추가적으로 이루어지는 처리는 다음과 같습니다.
 *		1. 타겟의 콜리전 비활성화
 *		2. 움직임 비활성화
 *		3. 어태치 대상으로 부터의 상대 회전 0으로 변경
 *		4. 클라이언트 서버 움직이 보정 비활성화
 *		5. 타겟의 카메라가 시전자의 카메라로 전환됨
 * 
 * 성공적으로 어빌리티가 종료되면 다음과 같은 태그 상태를 갖게 됩니다.
 * 시전자에게 추가되는 태그: 잡기 상태 태그
 * 타겟에게 추가되는 태그: 잡힌 상태 태그
 */
UCLASS()
class STEREOMIX_API USMGameplayAbility_Catch : public USMGameplayAbility
{
	GENERATED_BODY()

public:
	USMGameplayAbility_Catch();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Montage")
	TObjectPtr<UAnimMontage> CatchMontage;

protected:
	UFUNCTION()
	void OnCatchAnimNotify(FGameplayEventData Payload);

protected:
	UFUNCTION(Server, Reliable)
	void ServerRPCRequestCatch(const FVector_NetQuantize10& InStartLocation, const FVector_NetQuantize10& InCursorLocation);

	/** 잡을 수 있는 가장 적합한 타겟을 반환합니다.*/
	AActor* GetMostSuitableCatchableActor(const TArray<FOverlapResult>& InOverlapResults);

	/** 오버랩 결과에서 잡을 수 있는 액터를 추려냅니다. 잡을 수 있는 캐릭터가 없다면 false를 반환합니다. */
	bool GetCatchableActors(const TArray<FOverlapResult>& InOverlapResults, TArray<AActor*>& OutCatchableActors);

	/** 타겟이 유효한 각도 내에 존재하는 지 체크합니다. */
	bool IsValidateAngle(const AActor* TargetActor);

	/** 지정된 위치에 가장 가까운 캐릭터를 얻어냅니다. */
	AActor* GetClosestActorFromLocation(const TArray<AActor*>& InActors, const FVector& InLocation);

protected:
	/** 시전 위치입니다. */
	FVector StartLocation;

	/** 커서 위치입니다. */
	FVector TargetLocation;

	UPROPERTY(EditAnywhere, Category = "Catch")
	float MaxDistance = 300.0f;

	UPROPERTY(EditAnywhere, Category = "Catch")
	float LimitDegree = 90.0f;

	/** 방향에 구애받지 않고 무조건 잡을 수 있는 거리를 나타냅니다. */
	UPROPERTY(EditAnywhere, Category = "Catch", DisplayName = "무조건 잡을 수 있는 거리")
	float UnconditionallyCatchDistance = 150.0f;

	UPROPERTY(EditAnywhere, Category = "Debug")
	uint32 bShowDebug:1 = false;
};
