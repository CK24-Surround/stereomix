// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMUserWidget.h"
#include "Data/SMTeam.h"
#include "SMUserWidget_VictoryDefeat.generated.h"

class UButton;
class UCanvasPanel;
class UImage;

enum class EVictoryDefeatResult : uint8
{
	Victory,
	Defeat,
	Draw
};

/**
 * 
 */
UCLASS(Abstract)
class STEREOMIX_API USMUserWidget_VictoryDefeat : public USMUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	virtual void SetASC(UAbilitySystemComponent* InASC) override;

protected:
	void BindToGameState();

	void OnEndRound(ESMTeam VictoryTeam);

	void ShowResult(EVictoryDefeatResult InResult, ESMTeam InSourceTeam);

	void PlayResultAnimation(EVictoryDefeatResult InResult, ESMTeam InSourceTeam);

	TMap<EVictoryDefeatResult, TObjectPtr<UPanelWidget>> Panels;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCanvasPanel> VictoryPanel;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCanvasPanel> DefeatPanel;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCanvasPanel> DrawPanel;

	TMap<EVictoryDefeatResult, TMap<ESMTeam, TObjectPtr<UPanelWidget>>> TeamPanels;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCanvasPanel> VictoryBackgroundPanel_EDM;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCanvasPanel> VictoryBackgroundPanel_FB;

	TMap<EVictoryDefeatResult, TMap<ESMTeam, TObjectPtr<UWidgetAnimation>>> WidgetAnimations;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Victory_Ani_EDM;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Victory_Ani_FB;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Defeat_Ani;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Draw_Ani;
};
