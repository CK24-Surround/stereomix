// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameSession.h"
#include "SMRoomSession.generated.h"

UCLASS()
class STEREOMIX_API ASMRoomSession : public AGameSession
{
	GENERATED_BODY()

public:
	ASMRoomSession();

	virtual void RegisterServer() override;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
};
