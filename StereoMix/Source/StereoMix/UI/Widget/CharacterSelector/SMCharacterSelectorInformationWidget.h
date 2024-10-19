// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "SMCharacterSelectorInformationWidget.generated.h"

class UImage;
class UCommonTextBlock;
/**
 * 
 */
UCLASS()
class STEREOMIX_API USMCharacterSelectorInformationWidget : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	void ResetInfo() const;
	
	UFUNCTION(BlueprintCallable)
	void SetPlayerInfo(const TArray<FString>& InPlayerNames, const TArray<FString>& InPlayerCharacterTypes, const TArray<UTexture2D*>& InPlayerTextures);
	
	UFUNCTION(BlueprintCallable)
	void SetSkillInfo(const TArray<FString>& InSkillNames, const TArray<FString>& InSkillDescriptions, const TArray<UTexture2D*>& InSkillTextures);
	
protected:
	TArray<TObjectPtr<UImage>> SkillImages;

	TArray<TObjectPtr<UCommonTextBlock>> SkillNames;

	TArray<TObjectPtr<UCommonTextBlock>> SkillDescriptions;

	TArray<TObjectPtr<UImage>> PlayerImages;

	TArray<TObjectPtr<UCommonTextBlock>> PlayerNames;

	TArray<TObjectPtr<UCommonTextBlock>> PlayerCharacterTypes;

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
};
