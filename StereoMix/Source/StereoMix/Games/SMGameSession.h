// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameSession.h"
#include "SMGameSession.generated.h"

DECLARE_LOG_CATEGORY_CLASS(LogSMGameSession, Log, All);

UCLASS()
class STEREOMIX_API ASMGameSession : public AGameSession
{
	GENERATED_BODY()

public:
	ASMGameSession();

	virtual FString ApproveLogin(const FString& Options) override;

	virtual void OnStartSessionComplete(FName InSessionName, bool bWasSuccessful) override;

	virtual void OnEndSessionComplete(FName InSessionName, bool bWasSuccessful) override;
};
