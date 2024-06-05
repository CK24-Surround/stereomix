// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "Components/Image.h"
#include "Data/SMCharacterType.h"
#include "Data/SMTeam.h"
#include "SMCharacterPreviewWidget.generated.h"

USTRUCT(BlueprintType)
struct FCharacterPreviewWidgetData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ESMCharacterType CharacterType = ESMCharacterType::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UMaterialInstance> CharacterMaterial;

	FCharacterPreviewWidgetData() = default;

	FCharacterPreviewWidgetData(const FCharacterPreviewWidgetData& Other)
	{
		CharacterType = Other.CharacterType;
		CharacterMaterial = Other.CharacterMaterial;
	}
};

/**
 * 
 */
UCLASS()
class STEREOMIX_API USMCharacterPreviewWidget : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	void SetPreviewImage(ESMTeam Team, ESMCharacterType CharacterType);

private:
	// =============================================================================
	// Bindings
	
	UPROPERTY(Transient, BlueprintReadOnly, Category="Components", meta=(BindWidget, AllowPrivateAccess))
	TObjectPtr<UImage> PreviewImage;

	
	// =============================================================================
	// Variables
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Preview Materials", meta=(AllowPrivateAccess))
	TArray<FCharacterPreviewWidgetData> EdmCharacterPreviewDataList;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Preview Materials", meta=(AllowPrivateAccess))
	TArray<FCharacterPreviewWidgetData> FutureBassCharacterPreviewDataList;

	
};
