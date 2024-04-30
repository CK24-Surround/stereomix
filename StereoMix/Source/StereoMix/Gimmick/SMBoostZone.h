// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "GameFramework/Actor.h"
#include "SMBoostZone.generated.h"

class UAbilitySystemComponent;
class UGameplayEffect;
class UBoxComponent;

UCLASS()
class STEREOMIX_API ASMBoostZone : public AActor
{
	GENERATED_BODY()

public:
	ASMBoostZone();

protected:
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

protected:
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;

protected:
	void PerformBoostZone(USMAbilitySystemComponent* TargetASC);

	void ApplyBoostZone(USMAbilitySystemComponent* TargetASC);

	void RemoveBoostZone(USMAbilitySystemComponent* TargetASC);

protected:
	UPROPERTY(VisibleAnywhere, Category = "Scene")
	TObjectPtr<USceneComponent> SceneComponent;

	UPROPERTY(VisibleAnywhere, Category = "Collision")
	TObjectPtr<UBoxComponent> BoxComponent;

protected:
	UPROPERTY(EditAnywhere, Category = "GAS|GE")
	TSubclassOf<UGameplayEffect> BoostZoneGE;

	UPROPERTY(EditAnywhere, Category = "Boost")
	float MoveSpeedToApplyMultiply = 1.5f;

protected:
	struct FBoostZoneDirectionData
	{
		uint32 bOldIsCurrectDirection:1 = false;
		uint32 bNewIsCurrectDirection:1 = false;
	};

	/** 부스트 존 안에 들어온 플레이어의 ASC와 방향정보를 갖고 있는 맵입니다. */
	TMap<TWeakObjectPtr<USMAbilitySystemComponent>, FBoostZoneDirectionData> InZoneMap;
};
