// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Static.h"
#include "Data/SMLocalTeam.h"
#include "SMGameplayCueNotifyBase.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class STEREOMIX_API USMGameplayCueNotifyBase : public UGameplayCueNotify_Static
{
	GENERATED_BODY()

protected:
	/** 큐 파라미터에서 위치와 회전값을 가져옵니다.*/
	UFUNCTION(BlueprintCallable, Category = "Design|Utilities")
	void GetLocationAndRotation(const FGameplayCueParameters& Parameters, FVector& OutLocation, FRotator& OutRotation) const;

	/** 액터의 팀 정보를 반환합니다. */
	UFUNCTION(BlueprintCallable, Category = "Design|Utilities")
	ESMTeam GetTeam(const AActor* SourceActor) const;

	/** 액터의 로컬 팀 정보를 반환합니다. 대상이 유효하지 않은 경우에는 다른 팀으로 반환합니다. */
	UFUNCTION(BlueprintCallable, Category = "Design|Utilities")
	ESMLocalTeam GetLocalTeam(AActor* SourceActor) const;
};
