// Fill out your copyright notice in the Description page of Project Settings.

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

public:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	void UpdateMovementInfo();
	void UpdateDirection();

protected:
	UFUNCTION(BlueprintCallable, Category = "Movement", DisplayName = "Get Distance To Target")
	float K2_GetDistanceToTarget() const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation|Idle")
	TObjectPtr<UAnimSequence> Idle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation|WalkStart")
	TObjectPtr<UAnimSequence> ForwardStart;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation|WalkStart")
	TObjectPtr<UAnimSequence> BackStart;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation|WalkStart")
	TObjectPtr<UAnimSequence> RightStart;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation|WalkStart")
	TObjectPtr<UAnimSequence> LeftStart;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation|WalkCycle")
	TObjectPtr<UAnimSequence> ForwardWalkCycle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation|WalkCycle")
	TObjectPtr<UAnimSequence> BackWalkCycle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation|WalkCycle")
	TObjectPtr<UAnimSequence> RightWalkCycle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation|WalkCycle")
	TObjectPtr<UAnimSequence> LeftWalkCycle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation|WalkEnd")
	TObjectPtr<UAnimSequence> ForwardWalkEnd;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation|WalkEnd")
	TObjectPtr<UAnimSequence> BackWalkEnd;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation|WalkEnd")
	TObjectPtr<UAnimSequence> RightWalkEnd;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation|WalkEnd")
	TObjectPtr<UAnimSequence> LeftWalkEnd;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	TWeakObjectPtr<ASMPlayerCharacter> SourceCharacter;

	TWeakObjectPtr<UCharacterMovementComponent> SourceMovement;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	uint32 bHasAcceleration:1 = false;

	FVector Acceleration;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	uint32 bHasVeloicity:1 = false;

	FVector Velocity;

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
};
