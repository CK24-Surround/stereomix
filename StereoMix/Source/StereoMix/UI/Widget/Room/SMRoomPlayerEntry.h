// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Blueprint/UserWidget.h"
#include "CommonTextBlock.h"
#include "Components/Image.h"
#include "Games/Room/SMRoomPlayerState.h"

#include "SMRoomPlayerEntry.generated.h"

/**
 *
 */
UCLASS(Abstract)
class STEREOMIX_API USMRoomPlayerEntry : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	const FString& GetPlayerName() const;

	bool IsLocalPlayer() const;

	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

protected:
	UPROPERTY()
	TWeakObjectPtr<ASMRoomPlayerState> OwningPlayerState;

	virtual void NativePreConstruct() override;

private:
	// =============================================================================
	// Bindings

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Components", meta = (BindWidget, AllowPrivateAccess))
	TObjectPtr<UImage> BackgroundImage;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Components", meta = (BindWidget, AllowPrivateAccess))
	TObjectPtr<UCommonTextBlock> PlayerNameTextBlock;

	// =============================================================================
	// Style

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Style", meta = (AllowPrivateAccess))
	TObjectPtr<UTexture2D> BackgroundOnEmpty;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Style", meta = (AllowPrivateAccess))
	TObjectPtr<UTexture2D> BackgroundOnFilled;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Style", meta = (AllowPrivateAccess))
	TSubclassOf<UCommonTextStyle> LocalPlayerNameTextStyle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Style", meta = (AllowPrivateAccess))
	TSubclassOf<UCommonTextStyle> RemotePlayerNameTextStyle;
};
