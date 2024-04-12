// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SMGameMode.generated.h"

struct FProcessParameters;

DECLARE_LOG_CATEGORY_CLASS(LogGameLift, Log, All);

UCLASS()
class STEREOMIX_API ASMGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	// Begin GameLift Section
private:
	TSharedPtr<FProcessParameters> ProcessParameters;

	void InitGameLift();
	// End GameLift Section
};
