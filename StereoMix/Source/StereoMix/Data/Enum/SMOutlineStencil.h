// Copyright Studio Surround. All Rights Reserved.

#pragma once

UENUM(BlueprintType)
enum class ESMShaderStencil : uint8
{
	NonOutline = 0,
	SelfOutline = 1,
	EDMOutline = 2,
	FBOutline = 3,
	Hit = 4,
	Max UMETA(Hidden)
};
