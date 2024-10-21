// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "Data/SMCharacterType.h"
#include "SMCharacterSelectorInformationWidget.generated.h"

class UImage;
class UCommonTextBlock;
/**
 * 
 */
UCLASS(Abstract)
class STEREOMIX_API USMCharacterSelectorInformationWidget : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	void ResetInfo() const;

	void ResetPlayerInfo() const;

	UFUNCTION(BlueprintCallable)
	void SetPlayerInfo(const TArray<FString>& InPlayerNames, const TArray<ESMCharacterType>& InPlayerCharacterTypes);

	UFUNCTION(BlueprintCallable)
	void SetSkillInfo(ESMCharacterType InPlayerCharacterTypes);

	void SetPlayerReady(int32 PlayerIndex, bool bIsReady);

protected:
	UPROPERTY()
	TArray<TObjectPtr<UImage>> SkillImages;

	UPROPERTY()
	TArray<TObjectPtr<UCommonTextBlock>> SkillNames;

	UPROPERTY()
	TArray<TObjectPtr<UCommonTextBlock>> SkillDescriptions;

	UPROPERTY()
	TArray<TObjectPtr<UImage>> PlayerImages;

	UPROPERTY()
	TArray<TObjectPtr<UCommonTextBlock>> PlayerNames;

	UPROPERTY()
	TArray<TObjectPtr<UCommonTextBlock>> PlayerCharacterTypes;

	UPROPERTY()
	TArray<TObjectPtr<UWidgetAnimation>> PlayerReadyAnimations;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> SelectedCharacterType;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> SkillImage1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> SkillName1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> SkillDescription1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> SkillImage2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> SkillName2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> SkillDescription2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> SkillImage3;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> SkillName3;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> SkillDescription3;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> PlayerImage1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> PlayerName1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> PlayerCharacterType1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> PlayerImage2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> PlayerName2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> PlayerCharacterType2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> PlayerImage3;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> PlayerName3;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> PlayerCharacterType3;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Player1Ready;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Player2Ready;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Player3Ready;
};
