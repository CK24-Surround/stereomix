// Copyright Studio Surround. All Rights Reserved.

#pragma once

UENUM(BlueprintType)
enum class ESMRoundState : uint8
{
	None,
	PreRound,
	InRound,
	PostRound,
	Max UMETA(Hidden)
};
