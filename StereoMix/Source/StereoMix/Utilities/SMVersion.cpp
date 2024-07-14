// Copyright Surround, Inc. All Rights Reserved.


#include "SMVersion.h"

const FString VersionPatternStr = TEXT("^(0|[1-9]\\d*)\\.(0|[1-9]\\d*)\\.(0|[1-9]\\d*)(?:-((?:0|[1-9]\\d*|\\d*[a-zA-Z-][0-9a-zA-Z-]*)(?:\\.(?:0|[1-9]\\d*|\\d*[a-zA-Z-][0-9a-zA-Z-]*))*))?(?:\\+([0-9a-zA-Z-]+(?:\\.[0-9a-zA-Z-]+)*))?$");

FSMVersion::FSMVersion(const FString& InVersionString)
{
	static const FRegexPattern VersionPattern(VersionPatternStr);

	if (FRegexMatcher VersionMatcher(VersionPattern, InVersionString); VersionMatcher.FindNext())
	{
		Major = FCString::Atoi(*VersionMatcher.GetCaptureGroup(1));
		Minor = FCString::Atoi(*VersionMatcher.GetCaptureGroup(2));
		Patch = FCString::Atoi(*VersionMatcher.GetCaptureGroup(3));
		PreRelease = VersionMatcher.GetCaptureGroup(4);
		BuildMetadata = VersionMatcher.GetCaptureGroup(5);
	}
}

FString FSMVersion::ToString() const
{
	return FString::Printf(TEXT("%d.%d.%d%s%s"), Major, Minor, Patch, PreRelease.IsEmpty() ? TEXT("") : *FString::Printf(TEXT("-%s"), *PreRelease), BuildMetadata.IsEmpty() ? TEXT("") : *FString::Printf(TEXT("+%s"), *BuildMetadata));
}
