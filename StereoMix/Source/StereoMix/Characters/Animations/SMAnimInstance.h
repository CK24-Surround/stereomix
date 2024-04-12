// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SMAnimInstance.generated.h"

UENUM(BlueprintType)
enum class EDirection
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

public:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	void UpdateMovementInfo();
	void UpdateDirection();

protected:
	UPROPERTY()
	TWeakObjectPtr<ASMPlayerCharacter> SourceCharacter;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	uint32 bHasAcceleration:1 = false;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	FVector Acceleration;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	FVector Velocity;

	/** 현재 움직이는 방향 기준 회전 각도를 나태냅니다. */
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float LocalVelocityDirectionAngle = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float DeltaSpeed = 0.0f;

	float LastSpeed = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	EDirection Direction;

	// TODO: 4방향체크
	// TODO: 지난 프레임부터 현재 프레임까지 이동속도(이전 프레임의 속도와 현재 프레임의 속도 차이)
};
