// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMPlayerCharacterBase.h"
#include "SMBassCharacter.generated.h"

DECLARE_DELEGATE(FOnSlashEndSignature);

UCLASS(Abstract)
class STEREOMIX_API ASMBassCharacter : public ASMPlayerCharacterBase
{
	GENERATED_BODY()

	struct FSlashData
	{
		TArray<AActor*> DetectedActors;
		float SlashSpeed = 0.0f;
		float HalfAngle = 0.0f;
		uint32 bIsSlashing:1 = false;
		uint32 bIsForward:1 = false;
	};

public:
	ASMBassCharacter();

	virtual void PostInitializeComponents() override;

	virtual void Tick(float DeltaSeconds) override;

	void Slash(float Distance, float Angle, float SlashTime);

	FOnSlashEndSignature OnSlashEndSignature;

protected:
	void UpdateSlash();

	void SlashEnd();

	UFUNCTION()
	void OnSlashOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void ApplyDamage(AActor* TargetActor);

	UPROPERTY(VisibleAnywhere, Category = "Collider")
	TObjectPtr<USceneComponent> SlashColliderRootComponent;

	UPROPERTY(VisibleAnywhere, Category = "Collider")
	TObjectPtr<UCapsuleComponent> SlashColliderComponent;

	FSlashData SlashData;
};
