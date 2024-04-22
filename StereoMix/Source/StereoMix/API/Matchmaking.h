// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "JsonObjectConverter.h"
#include "MatchmakingModel.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Matchmaking.generated.h"

DECLARE_LOG_CATEGORY_CLASS(LogMatchmaking, Log, All)

namespace StereoMix::Matchmaking
{
	static const FString GBaseUrl = TEXT("https://qv9vw6bic7.execute-api.ap-northeast-2.amazonaws.com/Prod/");
	static const FString GRequestMatchUri = GBaseUrl + TEXT("matchmaking/request");
	static const FString GMatchStatusUri = GBaseUrl + TEXT("matchmaking/status");
}

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRequestMatchCompleteDelegate,
                                             bool, bSuccess, const FRequestMatchmakingResponse&, Response);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetMatchStatusCompleteDelegate,
                                             bool, bSuccess, const FGetMatchStatusResponse&, Response);

/**
 * Matchmaking Subsystem
 */
UCLASS()
class STEREOMIX_API UMatchmaking : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	void RequestMatch(const FString& PlayerName, const FString& Mode,
	                  const FOnRequestMatchCompleteDelegate& OnComplete);
	void GetMatchStatus(const FString& MatchId, const FOnGetMatchStatusCompleteDelegate& OnComplete);

private:
	template <typename T>
	bool ParseResponseContent(const FString& ResponseContent, T& Response)
	{
		check(IsInGameThread());

		TSharedPtr<FJsonObject> JsonObject;
		if (const TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(ResponseContent);
			!FJsonSerializer::Deserialize(JsonReader, JsonObject))
		{
			UE_LOG(LogMatchmaking, Error, TEXT("Failed to deserialize JSON response"));
			return false;
		}
		if (!FJsonObjectConverter::JsonObjectToUStruct<T>(JsonObject.ToSharedRef(), &Response))
		{
			UE_LOG(LogMatchmaking, Error, TEXT("Failed to convert JSON to Reseponse struct"));
			return false;
		}
		return true;
	}
};
