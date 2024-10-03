// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Data/Handle/SMMoveSpeedModifierHandle.h"
#include "SMCharacterMovementComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STEREOMIX_API USMCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

	struct FMoveSpeedModifierData
	{
		FSMMoveSpeedModifierHandle Handle;
		FTimerHandle TimerHandle;
		float ModifiedMoveSpeed;
	};

public:
	USMCharacterMovementComponent();

	virtual float GetMaxSpeed() const override;

	float GetBaseSpeed() const;

	FSMMoveSpeedModifierHandle AddMoveSpeedBuff(float SpeedMultiplier, float Duration);

	void RemoveMoveSpeedBuff(const FSMMoveSpeedModifierHandle& Handle);

	/**
	 * 서버에서 편하게 사용하기 위한 유틸리티 함수입니다. 이 함수는 핸들을 반환하지 못합니다. 따라서 조기 종료는 불가능합니다.
	 * 조기 종료가 필요하다면 다른 함수를 사용해주세요.
	 */
	UFUNCTION(Client, Reliable)
	void ClientAddMoveSpeedBuff(float SpeedMultiplier, float Duration);

protected:
	FSMMoveSpeedModifierHandle AddMoveSpeedModifier(float SpeedMultiplier, float Duration);

	void RemoveMoveSpeedModifier(const FSMMoveSpeedModifierHandle& Handle);

	TArray<FMoveSpeedModifierData> ActiveModifiers;

	TArray<int32> AvailableIDs;
	int32 NextModifierID = 1;

	float ModifierMoveSpeed = 0.0f;

private:
	UFUNCTION(Server, Reliable)
	void ServerAddMoveSpeedBuff(float SpeedMultiplier, float Duration);
};
