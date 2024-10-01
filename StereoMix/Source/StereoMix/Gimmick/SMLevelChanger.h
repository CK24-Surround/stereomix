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
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Root")
	TObjectPtr<USceneComponent> SceneComponent;

	UPROPERTY(VisibleAnywhere, Category = "Visual")
	TObjectPtr<UNiagaraComponent> NiagaraComponent;

	UPROPERTY(EditAnywhere, Category = "Design", meta = (ClampMin = "1"))
	int32 SwitchInterval = 10;

	UPROPERTY(EditAnywhere, Category = "Design", meta = (ClampMin = "0.0"))
	float SwitchOffset = 3.0f;

	UPROPERTY(EditAnywhere, Category = "Design", meta = (ClampMin = "1"))
	int32 ActiveLevelCount = 1;

	UPROPERTY(EditAnywhere, Category = "Design")
	TArray<TObjectPtr<UWorld>> SubLevels;

	TArray<TObjectPtr<UWorld>> ActiveSubLevels;

private:
	UFUNCTION(NetMulticast, Reliable)
	void MulticastShowActiveEffect();

	void SetRandomSubLevel();

	bool IsLevelActive(const TObjectPtr<UWorld>& SubLevel) const;

	void MarkLevelAsActive(const TObjectPtr<UWorld>& SubLevel);

	void SetLevelVisibility(const TObjectPtr<UWorld>& SubLevel, bool bVisibility);
};
