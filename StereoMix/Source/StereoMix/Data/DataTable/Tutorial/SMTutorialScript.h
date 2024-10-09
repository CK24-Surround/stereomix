// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "Data/SMCharacterType.h"

#include "SMTutorialScript.generated.h"

USTRUCT(BlueprintType)
struct FSMTutorialScript : public FTableRowBase
{
	GENERATED_BODY()

	FSMTutorialScript() {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Step = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESMCharacterType PlayerCharacterType = ESMCharacterType::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ScriptNumberInStep = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Ko;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString En;
};
