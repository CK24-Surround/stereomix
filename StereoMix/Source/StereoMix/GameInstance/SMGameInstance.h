// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class STEREOMIX_API USMGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	USMGameInstance();

	virtual void Init() override;

	virtual void StartGameInstance() override;

	static FString GetGameVersion();
};
