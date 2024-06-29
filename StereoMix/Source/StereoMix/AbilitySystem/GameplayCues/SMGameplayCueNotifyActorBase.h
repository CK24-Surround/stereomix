// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Actor.h"

#include "SMGameplayCueNotifyActorBase.generated.h"

UCLASS(Abstract)
class STEREOMIX_API ASMGameplayCueNotifyActorBase : public AGameplayCueNotify_Actor
{
	GENERATED_BODY()

public:
	ASMGameplayCueNotifyActorBase();

	/** 큐 파라미터에서 위치와 회전값을 가져옵니다.*/
	UFUNCTION(BlueprintCallable, Category = "Design|Utilities")
	void GetLocationAndRotation(const FGameplayCueParameters& Parameters, FVector& OutLocation, FRotator& OutRotation) const;
};
