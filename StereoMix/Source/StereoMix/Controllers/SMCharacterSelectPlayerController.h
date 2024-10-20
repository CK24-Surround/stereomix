// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Games/CharacterSelect/SMCharacterSelectState.h"
#include "SMPlayerController.h"
#include "UI/Widget/CharacterSelect/SMCharacterSelectWidget.h"
#include "UI/Widget/CharacterSelector/SMCharacterSelectorScreenWidget.h"
#include "UI/Widget/Loading/SMLoadingScreenWidget.h"

#include "SMCharacterSelectPlayerController.generated.h"

class ASMPreviewCharacter;
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
class STEREOMIX_API ASMCharacterSelectPlayerController : public ASMPlayerController
{
	GENERATED_BODY()

public:
	ASMCharacterSelectPlayerController();

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void OnRep_PlayerState() override;

	ASMCharacterSelectState* GetCharacterSelectState() const { return CharacterSelectState.Get(); }

	ASMCharacterSelectPlayerState* GetCharacterSelectPlayerState() const { return CharacterSelectPlayerState.Get(); }

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void RequestImmediateStartGame();

protected:
	void InitPlayer();

private:
	UFUNCTION()
	void OnCurrentCharacterSelectStateChanged(ECharacterSelectionStateType NewCharacterSelectionState);

	UFUNCTION()
	void OnCharacterChangeResponse(bool bSuccess, ESMCharacterType NewCharacterType);

	UPROPERTY(EditDefaultsOnly, Category = "Widget")
	TSubclassOf<USMLoadingScreenWidget> LoadingScreenWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Widget")
	TSubclassOf<USMCharacterSelectorScreenWidget> CharacterSelectWidgetClass;

	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Character", meta = (AllowPrivateAccess))
	TMap<ESMCharacterType, TObjectPtr<ASMPreviewCharacter>> PreviewCharacters;

	UPROPERTY(Transient)
	TObjectPtr<USMLoadingScreenWidget> LoadingScreenWidget;

	UPROPERTY(Transient)
	TObjectPtr<USMCharacterSelectorScreenWidget> CharacterSelectWidget;

	UPROPERTY()
	TWeakObjectPtr<ASMCharacterSelectState> CharacterSelectState;

	UPROPERTY()
	TWeakObjectPtr<ASMCharacterSelectPlayerState> CharacterSelectPlayerState;
};
