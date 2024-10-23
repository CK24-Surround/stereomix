// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "Data/SMCharacterType.h"
#include "Games/CharacterSelect/SMCharacterSelectState.h"
#include "SMCharacterSelectorScreenWidget.generated.h"

class UImage;
class UOverlay;
class USMCharacterSelectorTimerWidget;
class ASkeletalMeshActor;
class ASMCharacterSelectPlayerController;
class USMCharacterSelectorInformationWidget;
class ASMCharacterSelectPlayerState;
class ASMCharacterSelectState;
class UCommonTextBlock;
class UButton;
/**
 * 
 */
UCLASS(Abstract)
class STEREOMIX_API USMCharacterSelectorScreenWidget : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	void InitWidget(ASMCharacterSelectPlayerController* PlayerController, ASMCharacterSelectState* CharacterSelectState, ASMCharacterSelectPlayerState* PlayerState);

	ASMCharacterSelectState* GetOwningCharacterSelectState() const { return OwningCharacterSelectState.Get(); }

	ASMCharacterSelectPlayerState* GetOwningPlayerState() const { return OwningPlayerState.Get(); }

protected:
	UFUNCTION()
	void OnCountdownTick();

	UFUNCTION()
	void OnCurrentStateChanged(ECharacterSelectionStateType NewCharacterSelectionState);

	UFUNCTION()
	void OnPickElectricGuitar();

	UFUNCTION()
	void OnPickPiano();

	UFUNCTION()
	void OnPickBass();

	UFUNCTION()
	void OnSelectButtonClicked();

	UFUNCTION()
	void OnPlayerJoin(ASMPlayerState* JoinedPlayer);

	UFUNCTION()
	void OnPlayerLeft(ASMPlayerState* LeftPlayer);

	UFUNCTION()
	void OnPlayerCharacterChanged(ASMPlayerState* Player, ESMCharacterType NewCharacter);

	UFUNCTION()
	void OnCharacterChangeResponse(bool bSuccess, ESMCharacterType NewCharacterType);

	TArray<TObjectPtr<ASMPlayerState>> GetCurrentTeamPlayers() const;
	
	void ChangeFocusedCharacter(ESMCharacterType CharacterType);

	void ShowPreviewCharacter(ESMCharacterType CharacterType);

	void UpdatePlayerList() const;

	void UpdateSelectButton() const;

	bool IsFocusedCharacterSelectable(bool bExcludeOwner) const;

	void SetPlayerReady(ASMPlayerState* Player, const bool bIsReady) const;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> EDMProfiles;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> EDMElectricGuitar;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> EDMPiano;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> EDMBass;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> FBProfiles;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> FBElectricGuitar;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> FBPiano;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> FBBass;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> CharacterSelectBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SelectButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> SelectButtonText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> PickElectricGuitar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> PickPiano;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> PickBass;

	TWeakObjectPtr<USMCharacterSelectorInformationWidget> CharacterSelectorInformationWidget;

	TWeakObjectPtr<USMCharacterSelectorTimerWidget> CharacterSelectorTimerWidget;

	TWeakObjectPtr<ASMCharacterSelectPlayerController> OwningPlayerController;

	TWeakObjectPtr<ASMCharacterSelectState> OwningCharacterSelectState;

	TWeakObjectPtr<ASMCharacterSelectPlayerState> OwningPlayerState;

	UPROPERTY()
	TObjectPtr<ASkeletalMeshActor> CharacterMesh;

	FButtonStyle OriginalSelectButtonStyle;

	FButtonStyle OriginalSelectedButtonStyle;

	int32 bIsNeverSelected:1 = true;

	ESMCharacterType FocusedCharacterType = ESMCharacterType::None;

	TArray<TObjectPtr<ASMPlayerState>> CurrentTeamPlayers;
};
