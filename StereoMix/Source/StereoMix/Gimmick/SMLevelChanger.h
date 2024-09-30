// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/LevelStreaming.h"
#include "SMLevelChanger.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;

UCLASS(Abstract)
class STEREOMIX_API ASMLevelChanger : public AActor
{
	GENERATED_BODY()

public:
	ASMLevelChanger();

protected:
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

	UPROPERTY(VisibleAnywhere, Category = "Root")
	TObjectPtr<USceneComponent> SceneComponent;

	UPROPERTY(VisibleAnywhere, Category = "Visual")
	TObjectPtr<UNiagaraComponent> NiagaraComponent;

	UPROPERTY(VisibleAnywhere, Category = "Visual")
	TObjectPtr<UNiagaraSystem> ActiveNiagaraSystem;

	UPROPERTY(EditAnywhere, Category = "Design")
	TArray<TObjectPtr<UWorld>> SubLevels;

	UPROPERTY(EditAnywhere, Category = "Design")
	float SwitchInterval = 10.0f;

	UPROPERTY(EditAnywhere, Category = "Design")
	float LevelLifetime = 10.0f;

	float TimeSinceLastSwitch = 0.0f;

	FName CurrentSubLevel = NAME_None;

private:
	void SetRandomSubLevel();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastShowActiveEffect();

	void SetLevelVisibility(FName LevelName, bool bVisibility);
};
