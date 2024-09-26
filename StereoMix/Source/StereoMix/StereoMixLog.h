// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Logging/LogMacros.h"

class UObject;

STEREOMIX_API DECLARE_LOG_CATEGORY_EXTERN(LogStereoMix, Log, All)
STEREOMIX_API DECLARE_LOG_CATEGORY_EXTERN(LogStereoMixUI, Log, All)

STEREOMIX_API FString GetClientServerContextString(UObject* ContextObject = nullptr);