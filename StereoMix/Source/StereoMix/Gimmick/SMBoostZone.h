// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
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
	virtual void Tick(float DeltaSeconds) override;

protected:
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;

protected:
	void PerformBoostZone(UAbilitySystemComponent* TargetASC);

protected:
	UPROPERTY(VisibleAnywhere, Category = "Scene")
	TObjectPtr<USceneComponent> SceneComponent;

	UPROPERTY(VisibleAnywhere, Category = "Collision")
	TObjectPtr<UBoxComponent> BoxComponent;

protected:
	UPROPERTY(EditAnywhere, Category = "GAS|GE")
	TSubclassOf<UGameplayEffect> AddBoostZoneGE;

	UPROPERTY(EditAnywhere, Category = "GAS|GE")
	TSubclassOf<UGameplayEffect> RemoveBoostZoneGE;

protected:
	TArray<TWeakObjectPtr<UAbilitySystemComponent>> PlayerASCsInZone;
};
