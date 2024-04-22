// Fill out your copyright notice in the Description page of Project Settings.


#include "Matchmaking.h"

#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"

void UMatchmaking::RequestMatch(const FString& PlayerName, const FString& Mode,
                                const FOnRequestMatchCompleteDelegate& OnComplete)
{
	FHttpModule& HttpModule = FHttpModule::Get();
	const TSharedRef<IHttpRequest> Request = HttpModule.CreateRequest();

	Request->SetVerb(TEXT("GET"));
	// Request->SetHeader(TEXT("Content-Type"), TEXT("application/x-www-form-urlencoded"));

	const FString Uri = FString::Printf(
		TEXT("%s?playerName=%s&mode=%s"), *StereoMix::Matchmaking::GRequestMatchUri, *PlayerName, *Mode);

	UE_LOG(LogMatchmaking, Log, TEXT("RequestMatch Uri: %s"), *Uri);
	Request->SetURL(Uri);
	Request->OnProcessRequestComplete().BindLambda(
		[&](const FHttpRequestPtr& ProcessRequest, const FHttpResponsePtr& ProcessResponse,
		    const bool bConnectedSuccessfully)
		{
			if (bConnectedSuccessfully && ProcessResponse.IsValid())
			{
				const FString ResponseContent = ProcessResponse->GetContentAsString();
				UE_LOG(LogMatchmaking, Log, TEXT("RequestMatch Response: %s"), *ResponseContent);

				FRequestMatchmakingResponse MatchmakingResponse{};
				const bool bSuccess =
					ParseResponseContent<FRequestMatchmakingResponse>(ResponseContent, MatchmakingResponse);
				if (OnComplete.IsBound())
				{
					OnComplete.Broadcast(bSuccess, MatchmakingResponse);
				}
			}
			else
			{
				UE_LOG(LogMatchmaking, Error, TEXT("RequestMatch Failed"));
				if (OnComplete.IsBound())
				{
					OnComplete.Broadcast(false, FRequestMatchmakingResponse{});
				}
			}
		});

	Request->ProcessRequest();
}

void UMatchmaking::GetMatchStatus(const FString& MatchId, const FOnGetMatchStatusCompleteDelegate& OnComplete)
{
	FHttpModule& HttpModule = FHttpModule::Get();
	const TSharedRef<IHttpRequest> Request = HttpModule.CreateRequest();

	Request->SetVerb(TEXT("GET"));
	// Request->SetHeader(TEXT("Content-Type"), TEXT("application/x-www-form-urlencoded"));

	const FString Uri = FString::Printf(TEXT("%s?matchId=%s"), *StereoMix::Matchmaking::GMatchStatusUri, *MatchId);

	UE_LOG(LogMatchmaking, Log, TEXT("GetMatchStatus Uri: %s"), *Uri);
	Request->SetURL(Uri);
	Request->OnProcessRequestComplete().BindLambda(
		[&](const FHttpRequestPtr& ProcessRequest, const FHttpResponsePtr& ProcessResponse,
		    const bool bConnectedSuccessfully)
		{
			if (bConnectedSuccessfully && ProcessResponse.IsValid())
			{
				const FString ResponseContent = ProcessResponse->GetContentAsString();
				UE_LOG(LogMatchmaking, Log, TEXT("GetMatchStatus Response: %s"), *ResponseContent);

				FGetMatchStatusResponse Response{};
				const bool bSuccess = ParseResponseContent<FGetMatchStatusResponse>(ResponseContent, Response);
				if (OnComplete.IsBound())
				{
					OnComplete.Broadcast(bSuccess, Response);
				}
			}
			else
			{
				UE_LOG(LogMatchmaking, Error, TEXT("GetMatchStatus Failed"));
				if (OnComplete.IsBound())
				{
					OnComplete.Broadcast(false, FGetMatchStatusResponse{});
				}
			}
		});

	Request->ProcessRequest();
}
