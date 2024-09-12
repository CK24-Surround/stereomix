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

protected:
	FSMMoveSpeedModifierHandle AddMoveSpeedModifier(float SpeedMultiplier, float Duration);

	void RemoveMoveSpeedModifier(const FSMMoveSpeedModifierHandle& Handle);

	TArray<FMoveSpeedModifierData> ActiveModifiers;

	TArray<int32> AvailableIDs;
	int32 NextModifierID = 1;

	float ModifierMoveSpeed = 0.0f;
};
