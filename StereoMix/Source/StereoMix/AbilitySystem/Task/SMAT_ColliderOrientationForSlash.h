// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "SMAT_ColliderOrientationForSlash.generated.h"

DECLARE_DELEGATE_OneParam(FOnSlashHitActorDelegate, AActor* /* HitActor*/);

class USMGA_Slash;
class ASMBassCharacter;
class UCapsuleComponent;

/**
 * 
 */
UCLASS()
class STEREOMIX_API USMAT_ColliderOrientationForSlash : public UAbilityTask
{
	GENERATED_BODY()

public:
	USMAT_ColliderOrientationForSlash();

	static USMAT_ColliderOrientationForSlash* ColliderOrientationForSlash(UGameplayAbility* OwningAbility, float Range, float Angle, float TotalSlashTime, bool bShowDebug);

	virtual void Activate() override;

	virtual void TickTask(float DeltaTime) override;

	virtual void OnDestroy(bool bInOwnerFinished) override;

	FOnSlashHitActorDelegate OnSlashHit;

protected:
	UFUNCTION()
	void BeginOnOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void HandleHit(AActor* OtherActor);

	TWeakObjectPtr<USMGA_Slash> SourceAbility;

	TWeakObjectPtr<ASMBassCharacter> SourceCharacter;

	TWeakObjectPtr<USceneComponent> SourceSlashColliderRootComponent;

	TWeakObjectPtr<UCapsuleComponent> SourceSlashColliderComponent;

	TArray<TWeakObjectPtr<AActor>> DetectedActors;

	float Range = 0.0f;

	float Angle = 0.0f;

	float SlashTime = 0.0f;

	uint32 bShowDebug:1 = false;

	float SlashSpeed = 0.0f;

	float StartYaw = 0.0f;

	float TargetYaw = 0.0f;

	// 캡슐 컴포넌트의 중심과 끝점을 저장해둡니다. 움직인 캡슐 사이의 빈공간도 라인 트레이스로 체크하기 위해 사용합니다.
	TArray<FVector> Locations;
};
