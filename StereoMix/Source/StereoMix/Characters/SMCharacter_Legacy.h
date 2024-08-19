// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "SMCharacter_Legacy.generated.h"

class USMCharacterAssetData;

UCLASS()
class STEREOMIX_API ASMCharacter_Legacy : public ACharacter
{
	GENERATED_BODY()

public:
	ASMCharacter_Legacy();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	UPROPERTY()
	TObjectPtr<const USMCharacterAssetData> AssetData;
};
