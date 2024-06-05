// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMFrontendPlayerController.h"
#include "Characters/SMPreviewCharacter.h"
#include "Games/CharacterSelect/SMCharacterSelectState.h"
#include "Player/SMPlayerState.h"
#include "UI/Widget/CharacterSelect/SMCharacterSelectWidget.h"
#include "UI/Widget/Loading/SMLoadingScreenWidget.h"
#include "SMCharacterSelectPlayerController.generated.h"

class ASMCharacterSelectPlayerState;

USTRUCT(BlueprintType)
struct FPreviewCharacterData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character")
	ESMCharacterType CharacterType = ESMCharacterType::None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character")
	TSubclassOf<ASMPreviewCharacter> CharacterClass;
};

/**
 * 
 */
UCLASS()
class STEREOMIX_API ASMCharacterSelectPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ASMCharacterSelectPlayerController();

	virtual void BeginPlay() override;

	virtual void OnRep_PlayerState() override;

	ASMCharacterSelectState* GetCharacterSelectState() const { return CharacterSelectState.Get(); }
	
	ASMCharacterSelectPlayerState* GetCharacterSelectPlayerState() const { return CharacterSelectPlayerState.Get(); }

protected:
	void InitPlayer();
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Widget")
	TSubclassOf<USMLoadingScreenWidget> LoadingScreenWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Widget")
	TSubclassOf<USMCharacterSelectWidget> CharacterSelectWidgetClass;

	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Character", meta=(AllowPrivateAccess))
	TMap<ESMCharacterType, TObjectPtr<ASMPreviewCharacter>> PreviewCharacters;

	UPROPERTY(Transient)
	TObjectPtr<USMLoadingScreenWidget> LoadingScreenWidget;

	UPROPERTY(Transient)
	TObjectPtr<USMCharacterSelectWidget> CharacterSelectWidget;

	UPROPERTY()
	TWeakObjectPtr<ASMCharacterSelectState> CharacterSelectState;

	UPROPERTY()
	TWeakObjectPtr<ASMCharacterSelectPlayerState> CharacterSelectPlayerState;

	UFUNCTION()
	void OnCurrentCharacterSelectStateChanged(ECharacterSelectionStateType NewCharacterSelectionState);
	
	UFUNCTION()
	void OnCharacterChangeResponse(bool bSuccess, ESMCharacterType NewCharacterType);
};
