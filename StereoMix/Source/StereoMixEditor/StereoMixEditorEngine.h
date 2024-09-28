// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "Editor/UnrealEdEngine.h"
#include "StereoMixEditorEngine.generated.h"

class UObject;

UCLASS()
class UStereoMixEditorEngine : public UUnrealEdEngine
{
	GENERATED_BODY()

protected:
	virtual FGameInstancePIEResult PreCreatePIEInstances(const bool bAnyBlueprintErrors, const bool bStartInSpectatorMode, const float PIEStartTime, const bool bSupportsOnlinePIE, int32& InNumOnlinePIEInstances) override;
};
