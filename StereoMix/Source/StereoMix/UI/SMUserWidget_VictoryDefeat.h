// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMUserWidget.h"
#include "Data/SMTeam.h"
#include "SMUserWidget_VictoryDefeat.generated.h"

class UImage;
/**
 * 
 */
UCLASS()
class STEREOMIX_API USMUserWidget_VictoryDefeat : public USMUserWidget
{
	GENERATED_BODY()

public:
	virtual void SetASC(UAbilitySystemComponent* InASC) override;

protected:
	void BindToGameState();

	void OnEndRound(ESMTeam VictoryTeam);

	void ShowVictory();

	void ShowDefeat();

	void ShowDraw();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Victory;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Defeat;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Draw;
};
