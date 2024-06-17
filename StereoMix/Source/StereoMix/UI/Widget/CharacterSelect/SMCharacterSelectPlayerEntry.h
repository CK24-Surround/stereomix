// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonTextBlock.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Blueprint/UserWidget.h"
#include "Components/ContentWidget.h"
#include "SMCharacterSelectPlayerEntry.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class STEREOMIX_API USMCharacterSelectPlayerEntry : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

protected:
	virtual void NativePreConstruct() override;

	void SetReady(bool bReady);

private:
	// =============================================================================
	// Bindings

	UPROPERTY(Transient, BlueprintReadOnly, Category="Components", meta=(BindWidget, AllowPrivateAccess))
	TObjectPtr<UCommonTextBlock> PlayerNameTextBlock;

	UPROPERTY(Transient, BlueprintReadOnly, Category="Components", meta=(BindWidget, AllowPrivateAccess))
	TObjectPtr<UCommonTextBlock> CharacterNameTextBlock;

	UPROPERTY(Transient, BlueprintReadOnly, Category="Components", meta=(BindWidget, AllowPrivateAccess))
	TObjectPtr<UPanelWidget> ReadyIconSection;

	UPROPERTY(Transient, BlueprintReadOnly, Category="Components", meta=(BindWidget, AllowPrivateAccess))
	TObjectPtr<UPanelWidget> NotReadyIconSection;


	// =============================================================================
	// Style

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Style", meta=(AllowPrivateAccess))
	TSubclassOf<UCommonTextStyle> LocalPlayerNameTextStyle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Style", meta=(AllowPrivateAccess))
	TSubclassOf<UCommonTextStyle> RemotePlayerNameTextStyle;
};
