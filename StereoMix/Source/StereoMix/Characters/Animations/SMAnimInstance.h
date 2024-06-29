// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"

#include "SMAnimInstance.generated.h"

class UCharacterMovementComponent;

UENUM(BlueprintType)
enum class EDirection : uint8
{
	Forward,
	Backward,
	Right,
	Left
};

class ASMPlayerCharacter;
/**
 *
 */
UCLASS()
class STEREOMIX_API USMAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	void UpdateMovementInfo();

	void UpdateDirection();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation|Idle")
	TObjectPtr<UAnimSequence> Idle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation|Idle")
	TObjectPtr<UAnimSequence> CatchIdle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation|MoveCycle")
	TObjectPtr<UAnimSequence> ForwardMove;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation|MoveCycle")
	TObjectPtr<UAnimSequence> BackwardMove;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation|MoveCycle")
	TObjectPtr<UAnimSequence> RightMove;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation|MoveCycle")
	TObjectPtr<UAnimSequence> LeftMove;

	UPROPERTY(BlueprintReadOnly, Category = "Reference")
	TObjectPtr<ASMPlayerCharacter> SourceCharacter;

	UPROPERTY(BlueprintReadOnly, Category = "Reference")
	TObjectPtr<UCharacterMovementComponent> SourceMovement;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	uint32 bHasAcceleration : 1 = false;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	FVector Acceleration2D;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	uint32 bHasVeloicity : 1 = false;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	FVector Velocity2D;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float DisplacementSinceLastUpdate = 0.0f;

	FVector PreviousLocation;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float DisplacementSpeed = 0.0f;

	/** 현재 움직이는 방향 기준 회전 각도를 나태냅니다. */
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float LocalVelocityDirectionAngle = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	EDirection Direction;

	UPROPERTY(BlueprintReadOnly, Category = "State")
	uint32 bAmICatching:1 = false;
};
