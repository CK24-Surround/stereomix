#pragma once

#include "CoreMinimal.h"
#include "MatchmakingModel.generated.h"

UENUM(Blueprintable)
enum class EResponseResult : uint8
{
	Success,
	Failed,
	Error
};

UENUM(Blueprintable)
enum class EMatchStatusTypes : uint8
{
	Unknown,
	Searching,
	Found,
	Succeeded,
	TimedOut,
	Cancelled,
	Failed
};

UENUM(Blueprintable)
enum class EMatchmakingEventTypes : uint8
{
	MatchmakingSearching,
	PotentialMatchCreated,
	AcceptMatch,
	AcceptMatchCompleted,
	MatchmakingSucceeded,
	MatchmakingTimedOut,
	MatchmakingCancelled,
	MatchmakingFailed
};

USTRUCT(Blueprintable)
struct FRuleEvaluationMetrics
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite)
	FString RuleName;
	UPROPERTY(BlueprintReadWrite)
	int32 PassedCount{0};
	UPROPERTY(BlueprintReadWrite)
	int32 FailedCount{0};
};

USTRUCT(Blueprintable)
struct FMatchPlayer
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString PlayerId;
	UPROPERTY(BlueprintReadWrite)
	FString PlayerSessionId;
	UPROPERTY(BlueprintReadWrite)
	FString Team;
	UPROPERTY(BlueprintReadWrite)
	bool Accepted{false};
};

USTRUCT(Blueprintable)
struct FTicket
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite)
	FString TicketId;
	UPROPERTY(BlueprintReadWrite)
	FDateTime StartTime;
	UPROPERTY(BlueprintReadWrite)
	TArray<FMatchPlayer> Players;
};

USTRUCT(Blueprintable)
struct FMatchStatus
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite)
	FString IpAddress;
	UPROPERTY(BlueprintReadWrite)
	int32 Port{0};
	UPROPERTY(BlueprintReadWrite)
	FString PlayerId;
	UPROPERTY(BlueprintReadWrite)
	FString PlayerSessionId;
	UPROPERTY(BlueprintReadWrite)
	FDateTime UpdatedTime;
	UPROPERTY(BlueprintReadWrite)
	EMatchStatusTypes Status{EMatchStatusTypes::Unknown};
};

USTRUCT(Blueprintable)
struct FGameSessionInfo
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite)
	FString GameSessionArn;
	UPROPERTY(BlueprintReadWrite)
	FString IpAddress;
	UPROPERTY(BlueprintReadWrite)
	int32 Port{0};
	UPROPERTY(BlueprintReadWrite)
	TArray<FMatchPlayer> Players;
};

USTRUCT(Blueprintable)
struct FRequestMatchmakingResponse
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite)
	EResponseResult Result{EResponseResult::Error};
	UPROPERTY(BlueprintReadWrite)
	FString TicketId;
};

USTRUCT(Blueprintable)
struct FGetMatchStatusResponse
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite)
	EResponseResult Result{EResponseResult::Error};
	UPROPERTY(BlueprintReadWrite)
	FMatchStatus MatchStatus;
};
