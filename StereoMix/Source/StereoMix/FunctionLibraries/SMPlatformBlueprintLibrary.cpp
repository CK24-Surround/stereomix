// Copyright Surround, Inc. All Rights Reserved.

#include "SMPlatformBlueprintLibrary.h"

#if PLATFORM_WINDOWS
#include "Windows/WindowsPlatformApplicationMisc.h"
#elif PLATFORM_LINUX
#include "Linux/LinuxPlatformApplicationMisc.h"
#endif

void USMPlatformBlueprintLibrary::CopyToClipboard(const FString& Text)
{
	FPlatformApplicationMisc::ClipboardCopy(*Text);
}
