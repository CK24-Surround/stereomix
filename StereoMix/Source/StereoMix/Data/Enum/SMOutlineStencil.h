// Copyright Studio Surround. All Rights Reserved.

#pragma once

UENUM(BlueprintType)
enum class ESMShaderStencil : uint8
{
	NonOutline = 0,
	SelfOutline = 1,
	EnemyOutline = 2,
	Hit = 3,
	Max UMETA(Hidden)
};
