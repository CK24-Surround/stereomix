// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameLiftServerSDK.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameLift.generated.h"

//class Aws::GameLift::Server::Model::GameSession;
class FGameLiftServerSDKModule;
struct FProcessParameters;

DECLARE_LOG_CATEGORY_CLASS(LogGameLift, Log, All);

using Aws::GameLift::Server::Model::GameSession;
using Aws::GameLift::Server::Model::UpdateGameSession;

/**
 * GameLift Subsystem
 */
UCLASS()
class STEREOMIX_API UGameLift : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	bool bInitialized;
	TSharedPtr<FProcessParameters> ProcessParameters;
	FGameLiftServerSDKModule* SdkModule;

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

public:
	FSimpleDelegate OnTerminateFromGameLift;

	FORCEINLINE static bool IsAvailable()
	{
#if WITH_GAMELIFT
		return true;
#else
		return false;
#endif
	}

	UGameLift();

	FORCEINLINE bool IsInitialized() const { return bInitialized; }

	FGameLiftServerSDKModule* GetSDK() const { return SdkModule; }

	/**
	 * Initialize GameLift
	 */
	void InitSDK();
};
