// Fill out your copyright notice in the Description page of Project Settings.


#include "GameLift.h"
#if WITH_GAMELIFT
#include "GameLiftServerSDK.h"
#include "GameLiftServerSDKModels.h"
#endif

void UGameLift::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UE_LOG(LogGameLift, Log, TEXT("Initializing GameLift subsystem"));

#if WITH_GAMELIFT
	SdkModule = &FModuleManager::LoadModuleChecked<FGameLiftServerSDKModule>(FName("GameLiftServerSDK"));
#endif
}

UGameLift::UGameLift()
{
	SdkModule = nullptr;
	bInitialized = false;

#if WITH_GAMELIFT
	ProcessParameters = MakeShared<FProcessParameters>();
#else
	ProcessParameters = nullptr;
#endif

}

// ReSharper disable once CppMemberFunctionMayBeConst
// ReSharper disable once CppMemberFunctionMayBeStatic
void UGameLift::InitGameLift()
{
#if WITH_GAMELIFT
	if (bInitialized)
	{
		return;
	}
	bInitialized = true;

	UE_LOG(LogGameLift, Log, TEXT("Calling InitGameLift..."));

	// Getting the module first.
	FGameLiftServerSDKModule* GameLiftSdkModule = GetModule();

	//Define the server parameters for a GameLift Anywhere fleet. These are not needed for a GameLift managed EC2 fleet.
	FServerParameters ServerParametersForAnywhere;

	bool bIsAnywhereActive = false;
	if (FParse::Param(FCommandLine::Get(), TEXT("glAnywhere")))
	{
		bIsAnywhereActive = true;
	}

	if (bIsAnywhereActive)
	{
		UE_LOG(LogGameLift, Log, TEXT("Configuring server parameters for Anywhere..."));

		// If GameLift Anywhere is enabled, parse command line arguments and pass them in the ServerParameters object.
		FString glAnywhereWebSocketUrl = "";
		if (FParse::Value(FCommandLine::Get(), TEXT("glAnywhereWebSocketUrl="), glAnywhereWebSocketUrl))
		{
			ServerParametersForAnywhere.m_webSocketUrl = TCHAR_TO_UTF8(*glAnywhereWebSocketUrl);
		}

		FString glAnywhereFleetId = "";
		if (FParse::Value(FCommandLine::Get(), TEXT("glAnywhereFleetId="), glAnywhereFleetId))
		{
			ServerParametersForAnywhere.m_fleetId = TCHAR_TO_UTF8(*glAnywhereFleetId);
		}

		FString glAnywhereProcessId = "";
		if (FParse::Value(FCommandLine::Get(), TEXT("glAnywhereProcessId="), glAnywhereProcessId))
		{
			ServerParametersForAnywhere.m_processId = TCHAR_TO_UTF8(*glAnywhereProcessId);
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

		FString glAnywhereHostId = "";
		if (FParse::Value(FCommandLine::Get(), TEXT("glAnywhereHostId="), glAnywhereHostId))
		{
			ServerParametersForAnywhere.m_hostId = TCHAR_TO_UTF8(*glAnywhereHostId);
		}

		FString glAnywhereAuthToken = "";
		if (FParse::Value(FCommandLine::Get(), TEXT("glAnywhereAuthToken="), glAnywhereAuthToken))
		{
			ServerParametersForAnywhere.m_authToken = TCHAR_TO_UTF8(*glAnywhereAuthToken);
		}

		UE_LOG(LogGameLift, SetColor, TEXT("%s"), COLOR_YELLOW);
		UE_LOG(LogGameLift, Log, TEXT(">>>> Web Socket URL: %s"), *ServerParametersForAnywhere.m_webSocketUrl);
		UE_LOG(LogGameLift, Log, TEXT(">>>> Fleet ID: %s"), *ServerParametersForAnywhere.m_fleetId);
		UE_LOG(LogGameLift, Log, TEXT(">>>> Process ID: %s"), *ServerParametersForAnywhere.m_processId);
		UE_LOG(LogGameLift, Log, TEXT(">>>> Host ID (Compute Name): %s"), *ServerParametersForAnywhere.m_hostId);
		UE_LOG(LogGameLift, Log, TEXT(">>>> Auth Token: %s"), *ServerParametersForAnywhere.m_authToken);
		UE_LOG(LogGameLift, SetColor, TEXT("%s"), COLOR_NONE);
	}

	UE_LOG(LogGameLift, Log, TEXT("Initializing the GameLift Server..."));

	//InitSDK will establish a local connection with GameLift's agent to enable further communication.
	FGameLiftGenericOutcome InitSdkOutcome = GameLiftSdkModule->InitSDK(ServerParametersForAnywhere);
	if (InitSdkOutcome.IsSuccess())
	{
		UE_LOG(LogGameLift, SetColor, TEXT("%s"), COLOR_GREEN);
		UE_LOG(LogGameLift, Log, TEXT("GameLift InitSDK succeeded!"));
		UE_LOG(LogGameLift, SetColor, TEXT("%s"), COLOR_NONE);
	}
	else
	{
		UE_LOG(LogGameLift, SetColor, TEXT("%s"), COLOR_RED);
		UE_LOG(LogGameLift, Log, TEXT("ERROR: InitSDK failed : ("));
		FGameLiftError GameLiftError = InitSdkOutcome.GetError();
		UE_LOG(LogGameLift, Log, TEXT("ERROR: %s"), *GameLiftError.m_errorMessage);
		UE_LOG(LogGameLift, SetColor, TEXT("%s"), COLOR_NONE);
		return;
	}

	//When a game session is created, GameLift sends an activation request to the game server and passes along the game session object containing game properties and other settings.
	//Here is where a game server should take action based on the game session object.
	//Once the game server is ready to receive incoming player connections, it should invoke GameLiftServerAPI.ActivateGameSession()
	ProcessParameters->OnStartGameSession.BindLambda([=](const Aws::GameLift::Server::Model::GameSession& InGameSession)
		{
			const FString GameSessionId = FString(InGameSession.GetGameSessionId());
			UE_LOG(LogGameLift, Log, TEXT("GameSession Initializing: %s"), *GameSessionId);
			GameLiftSdkModule->ActivateGameSession();
		});

	//OnProcessTerminate callback. GameLift will invoke this callback before shutting down an instance hosting this game server.
	//It gives this game server a chance to save its state, communicate with services, etc., before being shut down.
	//In this case, we simply tell GameLift we are indeed going to shut down.
	ProcessParameters->OnTerminate.BindLambda([=]()
		{
			UE_LOG(LogGameLift, Log, TEXT("Game Server Process is terminating"));
			GameLiftSdkModule->ProcessEnding();
		});

	//This is the HealthCheck callback.
	//GameLift will invoke this callback every 60 seconds or so.
	//Here, a game server might want to check the health of dependencies and such.
	//Simply return true if healthy, false otherwise.
	//The game server has 60 seconds to respond with its health status. GameLift will default to 'false' if the game server doesn't respond in time.
	//In this case, we're always healthy!
	ProcessParameters->OnHealthCheck.BindLambda([]()
		{
			UE_LOG(LogGameLift, Log, TEXT("Performing Health Check"));
			return true;
		});

	//GameServer.exe -port=7777 LOG=server.mylog
	ProcessParameters->port = FURL::UrlConfig.DefaultPort;
	TArray<FString> CommandLineTokens;
	TArray<FString> CommandLineSwitches;

	FCommandLine::Parse(FCommandLine::Get(), CommandLineTokens, CommandLineSwitches);

	for (FString SwitchStr : CommandLineSwitches)
	{
		FString Key;
		FString Value;

		if (SwitchStr.Split("=", &Key, &Value))
		{
			if (Key.Equals("port"))
			{
				ProcessParameters->port = FCString::Atoi(*Value);
			}
		}
	}

	//Here, the game server tells GameLift where to find game session log files.
	//At the end of a game session, GameLift uploads everything in the specified
	//location and stores it in the cloud for access later.
	TArray<FString> Logfiles;
	Logfiles.Add(TEXT("GameServerLog/Saved/Logs/GameServerLog.log"));
	ProcessParameters->logParameters = Logfiles;

	//The game server calls ProcessReady() to tell GameLift it's ready to host game sessions.
	UE_LOG(LogGameLift, Log, TEXT("Calling Process Ready..."));
	FGameLiftGenericOutcome ProcessReadyOutcome = GameLiftSdkModule->ProcessReady(*ProcessParameters);

	if (ProcessReadyOutcome.IsSuccess())
	{
		UE_LOG(LogGameLift, SetColor, TEXT("%s"), COLOR_GREEN);
		UE_LOG(LogGameLift, Log, TEXT("Process Ready!"));
		UE_LOG(LogGameLift, SetColor, TEXT("%s"), COLOR_NONE);
	}
	else
	{
		UE_LOG(LogGameLift, SetColor, TEXT("%s"), COLOR_RED);
		UE_LOG(LogGameLift, Log, TEXT("ERROR: Process Ready Failed!"));
		FGameLiftError ProcessReadyError = ProcessReadyOutcome.GetError();
		UE_LOG(LogGameLift, Log, TEXT("ERROR: %s"), *ProcessReadyError.m_errorMessage);
		UE_LOG(LogGameLift, SetColor, TEXT("%s"), COLOR_NONE);
	}

	UE_LOG(LogGameLift, Log, TEXT("InitGameLift completed!"));
#else
	UE_LOG(LogGameLift, Log, TEXT("GameLift SDK is not enabled."))
#endif
}
