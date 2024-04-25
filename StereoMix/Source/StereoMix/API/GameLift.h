// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameLift.generated.h"

#if WITH_GAMELIFT
namespace Aws::GameLift::Server::Model
{
	class GameSession;
	class UpdateGameSession;
}

class FGameLiftServerSDKModule;
struct FProcessParameters;

using  Aws::GameLift::Server::Model::GameSession;
using  Aws::GameLift::Server::Model::UpdateGameSession;
#endif

DECLARE_LOG_CATEGORY_CLASS(LogGameLift, Log, All);

/**
 * GameLift Subsystem
 */
UCLASS()
class STEREOMIX_API UGameLift : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UGameLift();
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

#if WITH_GAMELIFT

private:
	bool bInitialized;
	FGameLiftServerSDKModule* SdkModule;
	TSharedPtr<FProcessParameters> ProcessParameters;

	static void PrintGameSessionInfo(const GameSession& GameSessionInfo);

public:
	FSimpleDelegate OnTerminateFromGameLift;

	FORCEINLINE FGameLiftServerSDKModule* GetSDK() const
	{
		return SdkModule;
	}
#endif

	// ReSharper disable once CppMemberFunctionMayBeStatic
	FORCEINLINE bool IsInitialized() const
	{
#if WITH_GAMELIFT
		return bInitialized;
#else
		return false;
#endif
	}

	/**
	 * Initialize GameLift
	 */
	void InitSDK();

	void ProcessReady() const;
};
