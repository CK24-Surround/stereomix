// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SMTeamBlueprintLibrary.generated.h"

/**
 * 
 */
UCLASS()
class STEREOMIX_API USMTeamBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** 로컬에서 플레이하고 있는 팀과 시전자의 팀이 같은지 확인합니다. */
	UFUNCTION(BlueprintCallable, Category = "Team")
	static bool IsSameTeam(AActor* SourceActor);
};
