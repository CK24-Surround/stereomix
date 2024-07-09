// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FMODBlueprintStatics.h"
#include "SMFMODBlueprintStatics.generated.h"

/**
 * 
 */
UCLASS()
class STEREOMIX_API USMFMODBlueprintStatics : public UFMODBlueprintStatics
{
	GENERATED_BODY()

public:
	/** 해당 Bus의 볼륨 값을 반환합니다. 실패시 0을 반환합니다.*/
	UFUNCTION(BlueprintCallable, Category = "Audio|FMOD|Bus", meta = (UnsafeDuringActorConstruction = "true"))
	static float BusGetVolume(UFMODBus* Bus);

	UFUNCTION(BlueprintCallable, Category = "Audio|FMOD|VCA", meta = (UnsafeDuringActorConstruction = "true"))
	static float VCAGetVolume(UFMODVCA* VCA);
};
