// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/Image.h"
#include "Data/SMCharacterType.h"
#include "Data/SMTeam.h"
#include "UI/Widget/Common/SMCommonButton.h"
#include "SMCharacterSelectButton.generated.h"

class USMCharacterSelectWidget;

USTRUCT(BlueprintType)
struct FSMCharacterSelectButtonData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character", meta=(AllowPrivateAccess))
	TObjectPtr<UTexture2D> DefaultTexture;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character", meta=(AllowPrivateAccess))
	TObjectPtr<UTexture2D> FocusedTexture;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character", meta=(AllowPrivateAccess))
	TObjectPtr<UTexture2D> DisabledTexture;

	FSMCharacterSelectButtonData() = default;

	FSMCharacterSelectButtonData(const FSMCharacterSelectButtonData& Other)
	{
		DefaultTexture = Other.DefaultTexture;
		FocusedTexture = Other.FocusedTexture;
		DisabledTexture = Other.DisabledTexture;
	}
};

/**
 * 
 */
UCLASS()
class STEREOMIX_API USMCharacterSelectButton : public USMCommonButton
{
	GENERATED_BODY()

public:
	void InitWidget(USMCharacterSelectWidget* Widget);

	USMCharacterSelectWidget* GetParentWidget() const { return ParentCharacterSelectWidget.Get(); }

protected:
	virtual void NativePreConstruct() override;

	virtual void NativeConstruct() override;

	virtual void NativeOnHovered() override;

	virtual void NativeOnUnhovered() override;

	virtual void NativeOnEnabled() override;

	virtual void NativeOnDisabled() override;

	virtual void NativeOnClicked() override;

	UFUNCTION()
	void OnCharacterChangeResponse(bool bSuccess, ESMCharacterType NewCharacterType);

private:
	// =============================================================================
	// Bindings

	UPROPERTY(Transient, BlueprintReadOnly, Category="Components", meta=(BindWidget, AllowPrivateAccess))
	TObjectPtr<UImage> ButtonImage;

	UPROPERTY(Transient, BlueprintReadOnly, Category="Components", meta=(BindWidget, AllowPrivateAccess))
	TObjectPtr<UImage> CharacterImage;

	UPROPERTY(Transient, BlueprintReadOnly, Category="Components", meta=(BindWidget, AllowPrivateAccess))
	TObjectPtr<UPanelWidget> InteractableSection;


	// =============================================================================
	// Animations

	UPROPERTY(Transient, BlueprintReadOnly, Category="Animation", meta=(BindWidgetAnim, AllowPrivateAccess))
	TObjectPtr<UWidgetAnimation> HoverAnim;


	// =============================================================================
	// Variables

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Button", meta=(AllowPrivateAccess))
	TObjectPtr<UTexture2D> DefaultButtonTexture;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Button", meta=(AllowPrivateAccess))
	TObjectPtr<UTexture2D> FocusedButtonTexture;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character", meta=(AllowPrivateAccess))
	FSMCharacterSelectButtonData EdmCharacterData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character", meta=(AllowPrivateAccess))
	FSMCharacterSelectButtonData FutureBassCharacterData;

	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly, Category="Character", meta=(AllowPrivateAccess))
	FSMCharacterSelectButtonData CharacterData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character", meta=(AllowPrivateAccess))
	bool bNotSupportedCharacter;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character", meta=(AllowPrivateAccess))
	ESMCharacterType CharacterType;

	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly, Category="Character", meta=(AllowPrivateAccess))
	ESMTeam Team;

	UPROPERTY()
	TWeakObjectPtr<USMCharacterSelectWidget> ParentCharacterSelectWidget;
};
