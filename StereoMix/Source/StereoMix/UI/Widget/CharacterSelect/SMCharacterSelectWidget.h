// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonListView.h"
#include "CommonTextBlock.h"
#include "CommonUserWidget.h"
#include "SMCharacterPreviewWidget.h"
#include "SMCharacterSelectButton.h"
#include "Components/Image.h"
#include "Games/CharacterSelect/SMCharacterSelectPlayerState.h"
#include "Games/CharacterSelect/SMCharacterSelectState.h"
#include "UI/Widget/Common/SMRoundProgressBarWidget.h"
#include "SMCharacterSelectWidget.generated.h"

USTRUCT(BlueprintType)
struct FSMCharacterSelectWidgetBackgroundData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Textures", meta=(AllowPrivateAccess))
	TObjectPtr<UTexture2D> Background1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Textures", meta=(AllowPrivateAccess))
	TObjectPtr<UTexture2D> Background2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Textures", meta=(AllowPrivateAccess))
	TObjectPtr<UTexture2D> Background3;

	FSMCharacterSelectWidgetBackgroundData() = default;
	
	FSMCharacterSelectWidgetBackgroundData(const FSMCharacterSelectWidgetBackgroundData& Other)
	{
		Background1 = Other.Background1;
		Background2 = Other.Background2;
		Background3 = Other.Background3;
	}
};

/**
 * 
 */
UCLASS()
class STEREOMIX_API USMCharacterSelectWidget : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	void InitWidget(ASMCharacterSelectState* CharacterSelectState, ASMCharacterSelectPlayerState* PlayerState);

	ASMCharacterSelectState* GetOwningCharacterSelectState() const { return OwningCharacterSelectState.Get(); }

	ASMCharacterSelectPlayerState* GetOwningPlayerState() const { return OwningPlayerState.Get(); }

	USMCharacterPreviewWidget* GetCharacterPreview() const { return CharacterPreview.Get(); }

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void UpdatePlayerList() const;

	UFUNCTION()
	void OnCharacterSelectStateChanged(ECharacterSelectionStateType NewCharacterSelectionState);
	
	UFUNCTION()
	void OnLocalPlayerCharacterChangeResponse(bool bSuccess, ESMCharacterType NewCharacterType);
	
	UFUNCTION()
	void OnPlayerCharacterChanged(ASMPlayerState* Player, ESMCharacterType NewCharacter);
	
	UFUNCTION()
	void OnPlayerJoin(ASMPlayerState* JoinedPlayer);

	UFUNCTION()
	void OnPlayerLeft(ASMPlayerState* LeftPlayer);
	
	UFUNCTION()
	void OnCountdownTick();
	
private:
	// =============================================================================
	// Bindings

	UPROPERTY(Transient, BlueprintReadOnly, Category="Components", meta=(BindWidget, AllowPrivateAccess))
	TObjectPtr<UImage> Background1Image;

	UPROPERTY(Transient, BlueprintReadOnly, Category="Components", meta=(BindWidget, AllowPrivateAccess))
	TObjectPtr<UImage> Background2Image;

	UPROPERTY(Transient, BlueprintReadOnly, Category="Components", meta=(BindWidget, AllowPrivateAccess))
	TObjectPtr<UImage> Background3Image;
	
	UPROPERTY(Transient, BlueprintReadOnly, Category="Components", meta=(BindWidget, AllowPrivateAccess))
	TObjectPtr<USMRoundProgressBarWidget> CountdownProgressBar;

	UPROPERTY(Transient, BlueprintReadOnly, Category="Components", meta=(BindWidget, AllowPrivateAccess))
	TObjectPtr<UCommonTextBlock> CountdownTextBlock;
	
	UPROPERTY(Transient, BlueprintReadOnly, Category="Components", meta=(BindWidget, AllowPrivateAccess))
	TObjectPtr<UCommonListView> PlayerListView;
	
	UPROPERTY(Transient, BlueprintReadOnly, Category="Components", meta=(BindWidget, AllowPrivateAccess))
	TObjectPtr<USMCharacterPreviewWidget> CharacterPreview;

	UPROPERTY(Transient, BlueprintReadOnly, Category="Components", meta=(BindWidget, AllowPrivateAccess))
	TObjectPtr<USMCharacterSelectButton> ElectricGuitarSelectButton;

	UPROPERTY(Transient, BlueprintReadOnly, Category="Components", meta=(BindWidget, AllowPrivateAccess))
	TObjectPtr<USMCharacterSelectButton> PianoSelectButton;

	UPROPERTY(Transient, BlueprintReadOnly, Category="Components", meta=(BindWidget, AllowPrivateAccess))
	TObjectPtr<USMCharacterSelectButton> BassSelectButton;
 
	// =============================================================================
	// Animations
	UPROPERTY(Transient, BlueprintReadOnly, Category="Animation", meta=(BindWidgetAnim, AllowPrivateAccess))
	TObjectPtr<UWidgetAnimation> TransitionAnim;
	

	// =============================================================================
	// Variables

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Background", meta=(AllowPrivateAccess))
	FSMCharacterSelectWidgetBackgroundData EdmBackgroundData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Background", meta=(AllowPrivateAccess))
	FSMCharacterSelectWidgetBackgroundData FutureBassBackgroundData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sound", meta=(AllowPrivateAccess))
	TObjectPtr<UFMODEvent> CountdownSound;
	
	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category="My Character", meta=(AllowPrivateAccess))
	ESMCharacterType CurrentCharacter;
	
	UPROPERTY()
	TWeakObjectPtr<ASMCharacterSelectState> OwningCharacterSelectState;

	UPROPERTY()
	TWeakObjectPtr<ASMCharacterSelectPlayerState> OwningPlayerState;
};
