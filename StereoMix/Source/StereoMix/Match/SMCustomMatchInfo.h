// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "SMCustomMatchInfo.generated.h"

UENUM(BlueprintType)
enum class ESMCustomMatchVisibleType : uint8
{
	Public,
	Private
};

UENUM(BlueprintType)
enum class ESMCustomMatchState : uint8
{
	Active,
	Playing
};

USTRUCT(BlueprintType)
struct FSMCustomMatchInfo
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FText MatchName;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	ESMCustomMatchState State = ESMCustomMatchState::Active;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	ESMCustomMatchVisibleType Visibility = ESMCustomMatchVisibleType::Public;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int CurrentPlayers = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int MaxPlayers = 0;
};
