// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/GameplayCues/SMGameplayCueNotifyActorBase.h"
#include "SMGCN_NoiseBreakBurst_Piano.generated.h"

UCLASS()
class STEREOMIX_API ASMGCN_NoiseBreakBurst_Piano : public ASMGameplayCueNotifyActorBase
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASMGCN_NoiseBreakBurst_Piano();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
