// Fill out your copyright notice in the Description page of Project Settings.

#include "GameLift.h"

#if WITH_GAMELIFT
#include "Kismet/GameplayStatics.h"
#include "GameLiftServerSDK.h"
#include "GameLiftServerSDKModels.h"
#endif

UGameLift::UGameLift()
{
#if WITH_GAMELIFT
	bInitialized = false;
	SdkModule = nullptr;
	ProcessParameters = MakeShared<FProcessParameters>();
#endif
}

void UGameLift::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UE_LOG(LogGameLift, Log, TEXT("Initializing GameLift subsystem"));

#if WITH_GAMELIFT
	SdkModule = &FModuleManager::LoadModuleChecked<FGameLiftServerSDKModule>(FName("GameLiftServerSDK"));
	FGameLiftStringOutcome GetSdkVersionOutcome = SdkModule->GetSdkVersion();
	if (!GetSdkVersionOutcome.IsSuccess())
	{
		UE_LOG(LogGameLift, Error,
		       TEXT("Failed to get GameLift SDK version: %s"), *GetSdkVersionOutcome.GetError().m_errorMessage);
	}
	UE_LOG(LogGameLift, Log, TEXT("GameLift SDK Version: %s"), *GetSdkVersionOutcome.GetResult());
#endif
}

#if WITH_GAMELIFT
void UGameLift::PrintGameSessionInfo(const GameSession& GameSessionInfo)
{
	UE_LOG(LogGameLift, Log, TEXT("\tName: %hs"), GameSessionInfo.GetName());
	UE_LOG(LogGameLift, Log, TEXT("\tIpAddress: %hs"), GameSessionInfo.GetIpAddress());
	UE_LOG(LogGameLift, Log, TEXT("\tPort: %d"), GameSessionInfo.GetPort());
	UE_LOG(LogGameLift, Log, TEXT("\tDnsName: %hs"), GameSessionInfo.GetDnsName());
	UE_LOG(LogGameLift, Log, TEXT("\tFleetId: %hs"), GameSessionInfo.GetFleetId());
	UE_LOG(LogGameLift, Log, TEXT("\tGameSessionId: %hs"), GameSessionInfo.GetGameSessionId());
	UE_LOG(LogGameLift, Log, TEXT("\tMaximumPlayerSessionCount: %d"), GameSessionInfo.GetMaximumPlayerSessionCount());
	UE_LOG(LogGameLift, Log, TEXT("\tMatchmakerData: %hs"), GameSessionInfo.GetMatchmakerData());
	UE_LOG(LogGameLift, Log, TEXT("\tGameSessionData: %hs"), GameSessionInfo.GetGameSessionData());

	int32 GamePropertiesCount = 0;
	const Aws::GameLift::Server::Model::GameProperty* GameProperties
		= GameSessionInfo.GetGameProperties(GamePropertiesCount);

	UE_LOG(LogGameLift, Log, TEXT("\tGamePropertiesCount: %d"), GamePropertiesCount);
	// for (int32 i = 0; i < GamePropertiesCount; i++)
	// {
	// 	UE_LOG(LogGameLift, Log,
	// 	       TEXT("\tGameProperty%d: %hs = %hs"), i, GameProperties[i].GetKey(), GameProperties[i].GetValue());
	// }
}
#endif

// ReSharper disable once CppMemberFunctionMayBeStatic
void UGameLift::InitSDK()
{
#if WITH_GAMELIFT
	// Getting the module first.
	FGameLiftServerSDKModule* GameLiftSdkModule = GetSDK();

	FGameLiftStringOutcome GetSdkVersionOutcome = GameLiftSdkModule->GetSdkVersion();
	if (!GetSdkVersionOutcome.IsSuccess())
	{
		UE_LOG(LogGameLift, Error,
		       TEXT("Failed to get GameLift SDK version: %s"), *GetSdkVersionOutcome.GetError().m_errorMessage);
	}
	UE_LOG(LogGameLift, Log, TEXT("GameLift SDK Version: %s"), *GetSdkVersionOutcome.GetResult());

	// Anywhere fleet
	bool bIsAnywhereActive = false;
	if (FParse::Param(FCommandLine::Get(), TEXT("glAnywhere")))
	{
		bIsAnywhereActive = true;
	}

	FGameLiftGenericOutcome InitSdkOutcome;

	// If GameLift Anywhere is enabled, parse command line arguments and pass them in the ServerParameters object.
	if (bIsAnywhereActive)
	{
		//Define the server parameters for a GameLift Anywhere fleet. These are not needed for a GameLift managed EC2 fleet.
		FServerParameters ServerParametersForAnywhere;
		UE_LOG(LogGameLift, Log, TEXT("Configuring server parameters for Anywhere..."));

		if (FString GameLiftAnywhereWebSocketUrl;
			FParse::Value(FCommandLine::Get(), TEXT("glAnywhereWebSocketUrl="), GameLiftAnywhereWebSocketUrl))
		{
			ServerParametersForAnywhere.m_webSocketUrl = TCHAR_TO_UTF8(*GameLiftAnywhereWebSocketUrl);
		}

		if (FString GameLiftAnywhereFleetId;
			FParse::Value(FCommandLine::Get(), TEXT("glAnywhereFleetId="), GameLiftAnywhereFleetId))
		{
			ServerParametersForAnywhere.m_fleetId = TCHAR_TO_UTF8(*GameLiftAnywhereFleetId);
		}

		if (FString GameLiftAnywhereProcessId;
			FParse::Value(FCommandLine::Get(), TEXT("glAnywhereProcessId="), GameLiftAnywhereProcessId))
		{
			ServerParametersForAnywhere.m_processId = TCHAR_TO_UTF8(*GameLiftAnywhereProcessId);
		}
		else
		{
			// If no ProcessId is passed as a command line argument, generate a randomized unique string.
			ServerParametersForAnywhere.m_processId =
				TCHAR_TO_UTF8(
					*FText::Format(
						FText::FromString("ProcessId_{0}"),
						FText::AsNumber(FDateTime::UtcNow().GetTicks())
					).ToString()
				);
		}

		if (FString GameLiftAnywhereHostId;
			FParse::Value(FCommandLine::Get(), TEXT("glAnywhereHostId="), GameLiftAnywhereHostId))
		{
			ServerParametersForAnywhere.m_hostId = TCHAR_TO_UTF8(*GameLiftAnywhereHostId);
		}

		if (FString GameLiftAnywhereAuthToken;
			FParse::Value(FCommandLine::Get(), TEXT("glAnywhereAuthToken="), GameLiftAnywhereAuthToken))
		{
			ServerParametersForAnywhere.m_authToken = TCHAR_TO_UTF8(*GameLiftAnywhereAuthToken);
		}

		UE_LOG(LogGameLift, SetColor, TEXT("%s"), COLOR_YELLOW);
		UE_LOG(LogGameLift, Log, TEXT(">>>> Web Socket URL: %s"), *ServerParametersForAnywhere.m_webSocketUrl);
		UE_LOG(LogGameLift, Log, TEXT(">>>> Fleet ID: %s"), *ServerParametersForAnywhere.m_fleetId);
		UE_LOG(LogGameLift, Log, TEXT(">>>> Process ID: %s"), *ServerParametersForAnywhere.m_processId);
		UE_LOG(LogGameLift, Log, TEXT(">>>> Host ID (Compute Name): %s"), *ServerParametersForAnywhere.m_hostId);
		UE_LOG(LogGameLift, Log, TEXT(">>>> Auth Token: %s"), *ServerParametersForAnywhere.m_authToken);
		UE_LOG(LogGameLift, SetColor, TEXT("%s"), COLOR_NONE);

		UE_LOG(LogGameLift, Log, TEXT("Initializing the GameLift Anywhere Server..."));
		InitSdkOutcome = GameLiftSdkModule->InitSDK(ServerParametersForAnywhere);
	}
	else
	{
		UE_LOG(LogGameLift, Log, TEXT("Initializing the GameLift Server..."));
		InitSdkOutcome = GameLiftSdkModule->InitSDK();
	}

	if (!InitSdkOutcome.IsSuccess())
	{
		UE_LOG(LogGameLift, SetColor, TEXT("%s"), COLOR_RED);
		UE_LOG(LogGameLift, Log, TEXT("ERROR: InitSDK failed : ("));
		FGameLiftError GameLiftError = InitSdkOutcome.GetError();
		UE_LOG(LogGameLift, Log, TEXT("ERROR: %s"), *GameLiftError.m_errorMessage);
		UE_LOG(LogGameLift, SetColor, TEXT("%s"), COLOR_NONE);
		return;
	}

	UE_LOG(LogGameLift, SetColor, TEXT("%s"), COLOR_GREEN);
	UE_LOG(LogGameLift, Log, TEXT("GameLift InitSDK succeeded!"));
	UE_LOG(LogGameLift, SetColor, TEXT("%s"), COLOR_NONE);


	bInitialized = true;
#else
	UE_LOG(LogGameLift, Log, TEXT("GameLift SDK is not enabled."))
#endif
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void UGameLift::ProcessReady() const
{
#if WITH_GAMELIFT
	FGameLiftServerSDKModule* GameLiftSdkModule = GetSDK();

	ProcessParameters->OnStartGameSession.BindLambda([&](const GameSession& InGameSession)
	{
		const FString GameSessionId = FString(InGameSession.GetGameSessionId());
		UE_LOG(LogGameLift, Verbose, TEXT("GameLift GameSession started.: %s"), *GameSessionId);
		PrintGameSessionInfo(InGameSession);

		FGameLiftGenericOutcome ActivateOutcome = GameLiftSdkModule->ActivateGameSession();
		if (!ActivateOutcome.IsSuccess())
		{
			UE_LOG(LogGameLift, Error,
			       TEXT("Failed to activate game session: %s"), *ActivateOutcome.GetError().m_errorMessage);
		}
		UE_LOG(LogGameLift, Log, TEXT("GameLift GameSession activated."));
	});

	ProcessParameters->OnUpdateGameSession.BindLambda([&](const UpdateGameSession& InUpdateGameSession)
	{
		UE_LOG(LogGameLift, Log, TEXT("GameLift GameSession updated."));
		switch (InUpdateGameSession.GetUpdateReason())
		{
		case Aws::GameLift::Server::Model::UpdateReason::UNKNOWN:
			UE_LOG(LogGameLift, Log, TEXT("\tReason: UNKNOWN"));
			break;
		case Aws::GameLift::Server::Model::UpdateReason::MATCHMAKING_DATA_UPDATED:
			UE_LOG(LogGameLift, Log, TEXT("\tReason: MATCHMAKING_DATA_UPDATED"));
			break;
		case Aws::GameLift::Server::Model::UpdateReason::BACKFILL_FAILED:
			UE_LOG(LogGameLift, Log, TEXT("\tReason: BACKFILL_FAILED"));
			break;
		case Aws::GameLift::Server::Model::UpdateReason::BACKFILL_TIMED_OUT:
			UE_LOG(LogGameLift, Log, TEXT("\tReason: BACKFILL_TIMED_OUT"));
			break;
		case Aws::GameLift::Server::Model::UpdateReason::BACKFILL_CANCELLED:
			UE_LOG(LogGameLift, Log, TEXT("\tReason: BACKFILL_CANCELLED"));
			break;
		}

		const GameSession GameSessionInfo = InUpdateGameSession.GetGameSession();
		PrintGameSessionInfo(GameSessionInfo);
	});

	ProcessParameters->OnTerminate.BindLambda([&]
	{
		UE_LOG(LogGameLift, Verbose, TEXT("Game Server Process is terminating"));
		bool _ = OnTerminateFromGameLift.ExecuteIfBound();
	});

	ProcessParameters->OnHealthCheck.BindLambda([]
	{
		UE_LOG(LogGameLift, Verbose, TEXT("Performing Health Check"));
		return true;
	});

	//GameServer.exe -port=7777 LOG=server.mylog
	ProcessParameters->port = FURL::UrlConfig.DefaultPort;

	if (!FParse::Value(FCommandLine::Get(), TEXT("port="), ProcessParameters->port))
	{
		ProcessParameters->port = 7777;
	}

	//Here, the game server tells GameLift where to find game session log files.
	//At the end of a game session, GameLift uploads everything in the specified
	//location and stores it in the cloud for access later.
	TArray<FString> Logfiles;
	Logfiles.Add(TEXT("StereoMix/Saved/Logs/StereoMix.log"));
	ProcessParameters->logParameters = Logfiles;

	//The game server calls ProcessReady() to tell GameLift it's ready to host game sessions.
	UE_LOG(LogGameLift, Log, TEXT("Calling Process Ready..."));

	FGameLiftGenericOutcome ProcessReadyOutcome = GameLiftSdkModule->ProcessReady(*ProcessParameters);
	if (!ProcessReadyOutcome.IsSuccess())
	{
		UE_LOG(LogGameLift, SetColor, TEXT("%s"), COLOR_RED);
		UE_LOG(LogGameLift, Log, TEXT("ERROR: Process Ready Failed!"));
		FGameLiftError ProcessReadyError = ProcessReadyOutcome.GetError();
		UE_LOG(LogGameLift, Log, TEXT("ERROR: %s"), *ProcessReadyError.m_errorMessage);
		UE_LOG(LogGameLift, SetColor, TEXT("%s"), COLOR_NONE);
		return;
	}

	UE_LOG(LogGameLift, SetColor, TEXT("%s"), COLOR_GREEN);
	UE_LOG(LogGameLift, Log, TEXT("Process Ready!"));
	UE_LOG(LogGameLift, SetColor, TEXT("%s"), COLOR_NONE);
#endif
}
