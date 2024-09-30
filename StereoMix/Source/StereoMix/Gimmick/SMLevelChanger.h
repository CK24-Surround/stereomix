// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/LevelStreaming.h"
#include "SMLevelChanger.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;

UCLASS()
class STEREOMIX_API ASMLevelChanger : public AActor
{
	GENERATED_BODY()

public:
	ASMLevelChanger();

protected:
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

	UFUNCTION(Server, Reliable)
	void ServerSetLevelVisibility(FName LevelName, bool bVisible);
	
	UPROPERTY(VisibleAnywhere, Category = "Root")
	TObjectPtr<USceneComponent> SceneComponent;
	
	UPROPERTY(VisibleAnywhere, Category = "Visual")
	TObjectPtr<UNiagaraComponent> NiagaraComponent;

	UPROPERTY(VisibleAnywhere, Category = "Visual")
	TObjectPtr<UNiagaraSystem> ActiveNiagaraSystem;

	UPROPERTY(EditAnywhere, Category = "Design")
	FName SubLevel1;

	UPROPERTY(EditAnywhere, Category = "Design")
	FName SubLevel2;

	UPROPERTY(EditAnywhere, Category = "Design")
	float SwitchInterval = 10.0f;

	UPROPERTY(EditAnywhere, Category = "Design")
	float LevelLifetime = 10.0f;

	float TimeSinceLastSwitch = 0.0f;

	uint8 bIsSubLevel1Active:1 = true;

private:
	void SwitchLevels();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastShowActiveEffect();

	void SetLevelVisibility(FName LevelName, bool bVisibility);
};
