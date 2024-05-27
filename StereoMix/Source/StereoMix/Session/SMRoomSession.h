// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMGameSession.h"
#include "SMRoomSession.generated.h"

UCLASS()
class STEREOMIX_API ASMRoomSession : public ASMGameSession
{
	GENERATED_BODY()

public:
	ASMRoomSession();

	virtual void RegisterServer() override;

	virtual void InitOptions(const FString& Options) override;
	
protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
};
