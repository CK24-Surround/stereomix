// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "SMVersion.generated.h"

USTRUCT()
struct FSMVersion
{
	GENERATED_BODY()

	FSMVersion() = default;

	FSMVersion(const FSMVersion& Other)
		: Major(Other.Major), Minor(Other.Minor), Patch(Other.Patch), PreRelease(Other.PreRelease), BuildMetadata(Other.BuildMetadata)
	{
	}

	FSMVersion(FSMVersion&& Other) noexcept
		: Major(Other.Major), Minor(Other.Minor), Patch(Other.Patch), PreRelease(MoveTemp(Other.PreRelease)), BuildMetadata(MoveTemp(Other.BuildMetadata))
	{
	}

	FSMVersion(const FString& InVersionString);

	FSMVersion(int32 InMajor, int32 InMinor, int32 InPatch, const FString& InPreRelease, const FString& InBuildMetadata)
		: Major(InMajor), Minor(InMinor), Patch(InPatch), PreRelease(InPreRelease), BuildMetadata(InBuildMetadata)
	{
	};

	FSMVersion& operator=(const FSMVersion& Other)
	{
		if (this == &Other)
		{
			return *this;
		}
		Major = Other.Major;
		Minor = Other.Minor;
		Patch = Other.Patch;
		PreRelease = Other.PreRelease;
		BuildMetadata = Other.BuildMetadata;
		return *this;
	}

	FSMVersion& operator=(FSMVersion&& Other) noexcept
	{
		if (this == &Other)
		{
			return *this;
		}
		Major = Other.Major;
		Minor = Other.Minor;
		Patch = Other.Patch;
		PreRelease = MoveTemp(Other.PreRelease);
		BuildMetadata = MoveTemp(Other.BuildMetadata);
		return *this;
	}

	friend bool operator==(const FSMVersion& Lhs, const FSMVersion& Rhs)
	{
		return Lhs.Major == Rhs.Major && Lhs.Minor == Rhs.Minor && Lhs.Patch == Rhs.Patch && Lhs.PreRelease == Rhs.PreRelease && Lhs.BuildMetadata == Rhs.BuildMetadata;
	}

	friend bool operator!=(const FSMVersion& Lhs, const FSMVersion& Rhs)
	{
		return !(Lhs == Rhs);
	}

	friend bool operator<(const FSMVersion& Lhs, const FSMVersion& Rhs)
	{
		if (Lhs.Major != Rhs.Major)
		{
			return Lhs.Major < Rhs.Major;
		}
		if (Lhs.Minor != Rhs.Minor)
		{
			return Lhs.Minor < Rhs.Minor;
		}

		return Lhs.Patch < Rhs.Patch;
	}

	friend bool operator<=(const FSMVersion& Lhs, const FSMVersion& Rhs)
	{
		return Lhs == Rhs || Lhs < Rhs;
	}

	friend bool operator>(const FSMVersion& Lhs, const FSMVersion& Rhs)
	{
		return Rhs < Lhs;
	}

	friend bool operator>=(const FSMVersion& Lhs, const FSMVersion& Rhs)
	{
		return Rhs == Lhs || Rhs < Lhs;
	}

	FString ToString() const;

	int32 Major = 0;
	int32 Minor = 0;
	int32 Patch = 0;
	FString PreRelease;
	FString BuildMetadata;
};
