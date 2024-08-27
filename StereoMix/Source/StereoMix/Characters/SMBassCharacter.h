// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMPlayerCharacterBase.h"
#include "SMBassCharacter.generated.h"

DECLARE_DELEGATE(FOnSlashSignature);

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
	};

public:
	ASMBassCharacter();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PostInitializeComponents() override;

	virtual void Tick(float DeltaSeconds) override;

	void Slash(float Distance, float Angle, float SlashTime);

	bool GetIsLeftSlash() { return bIsLeftSlash; }

	void SetIsLeftSlash(bool bNewIsLeftSlash) { bIsLeftSlash = bNewIsLeftSlash; }

	bool GetCanInput() { return bCanInput; }

	void SetCanInput(bool bNewCanInput) { bCanInput = bNewCanInput; }

	bool GetCanNextAction() { return bCanNextAction; }

	void SetCanNextAction(bool bNewCanNextAction) { bCanNextAction = bNewCanNextAction; }

	bool GetIsInput() { return bIsInput; }

	void SetIsInput(bool bNewIsInput) { bIsInput = bNewIsInput; }

	FOnSlashSignature OnSlash;

protected:
	void UpdateSlash();

	UFUNCTION()
	void OnSlashOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void ApplyDamage(AActor* TargetActor);

	UPROPERTY(VisibleAnywhere, Category = "Collider")
	TObjectPtr<USceneComponent> SlashColliderRootComponent;

	UPROPERTY(VisibleAnywhere, Category = "Collider")
	TObjectPtr<UCapsuleComponent> SlashColliderComponent;

	UPROPERTY(Replicated)
	uint32 bIsLeftSlash:1 = true;

	UPROPERTY(Replicated)
	uint32 bCanInput:1 = false;

	UPROPERTY(Replicated)
	uint32 bCanNextAction:1 = false;

	uint32 bIsInput:1 = false;

	FSlashData SlashData;
};
