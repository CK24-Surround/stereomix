// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameLift.generated.h"

class FGameLiftServerSDKModule;
struct FProcessParameters;

DECLARE_LOG_CATEGORY_CLASS(LogGameLift, Log, All);

/**
 * GameLift Subsystem
 */
UCLASS()
class STEREOMIX_API UGameLift : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	bool bInitialized;
	FGameLiftServerSDKModule* SdkModule;


	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

public:
	TSharedPtr<FProcessParameters> ProcessParameters;

	UGameLift();
	FORCEINLINE bool IsInitialized() const { return bInitialized; }
	FORCEINLINE FGameLiftServerSDKModule* GetModule() const { return SdkModule; }

	/**
	 * Initialize GameLift
	 */
	void InitGameLift();
};
