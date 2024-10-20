// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "Games/CharacterSelect/SMCharacterSelectState.h"
#include "SMCharacterSelectorScreenWidget.generated.h"

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
	void InitWidget(ASMCharacterSelectState* CharacterSelectState, ASMCharacterSelectPlayerState* PlayerState);

	ASMCharacterSelectState* GetOwningCharacterSelectState() const { return OwningCharacterSelectState.Get(); }

	ASMCharacterSelectPlayerState* GetOwningPlayerState() const { return OwningPlayerState.Get(); }

protected:
	UFUNCTION()
	void OnPickElectricGuitar();

	UFUNCTION()
	void OnPickPiano();

	UFUNCTION()
	void OnPickBass();
	
	UFUNCTION()
	void OnPlayerJoin(ASMPlayerState* JoinedPlayer);
	
	UFUNCTION()
	void OnPlayerLeft(ASMPlayerState* LeftPlayer);
	
	UFUNCTION()
	void OnPlayerCharacterChanged(ASMPlayerState* Player, ESMCharacterType NewCharacter);

	UFUNCTION()
	void OnCharacterChangeResponse(bool bSuccess, ESMCharacterType NewCharacterType);

	void UpdatePlayerList() const;

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

	TObjectPtr<USMCharacterSelectorInformationWidget> CharacterSelectorInformationWidget;

	UPROPERTY()
	TWeakObjectPtr<ASMCharacterSelectState> OwningCharacterSelectState;

	UPROPERTY()
	TWeakObjectPtr<ASMCharacterSelectPlayerState> OwningPlayerState;
};
