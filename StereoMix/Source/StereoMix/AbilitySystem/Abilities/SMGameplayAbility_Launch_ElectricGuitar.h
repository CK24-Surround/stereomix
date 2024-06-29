// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMGameplayAbility_Launch_Custom.h"

#include "SMGameplayAbility_Launch_ElectricGuitar.generated.h"

struct FElectricGuitarLaunchData
{
	TArray<FVector> SpawnLocationOffsets;
	TArray<FRotator> SpawnRotationOffsets;
	int32 Count = 0;
	float Rate = 0.0f;
};

/**
 *
 */
UCLASS(Abstract)
class STEREOMIX_API USMGameplayAbility_Launch_ElectricGuitar : public USMGameplayAbility_Launch_Custom
{
	GENERATED_BODY()

public:
	USMGameplayAbility_Launch_ElectricGuitar();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	void LaunchProjectile();

	void LaunchProjectileTimerCallback();

	/** 투사체를 발사합니다. LaunchCount는 몇 번째 발사인지 나타냅니다. */
	UFUNCTION(Server, Reliable)
	void ServerRPCLaunchProjectile(const FVector_NetQuantize10& SourceLocation, const FVector_NetQuantizeNormal& Normal, int LaunchCount);

	UPROPERTY(EditAnywhere, Category = "Design|ElectricGuitar", DisplayName = "시전 당 투사체 개수")
	int32 ProjectileCount = 3;

	UPROPERTY(EditAnywhere, Category = "Design|ElectricGuitar", DisplayName = "투사체가 발사될 넓이")
	float ProjectileWidth = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Design|ElectricGuitar", DisplayName = "투사체가 발사될 각도")
	float ProjectileTotalAngle = 5.0f;

	UPROPERTY(EditAnywhere, Category = "Design|ElectricGuitar", DisplayName = "전탄 발사 소요 시간")
	float TotalLaunchTime = 0.175f;

	FElectricGuitarLaunchData LaunchData;
};