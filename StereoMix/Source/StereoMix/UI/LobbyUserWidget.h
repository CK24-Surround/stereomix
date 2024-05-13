// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyUserWidget.generated.h"

class UButton;
/**
 * 
 */
UCLASS()
class STEREOMIX_API ULobbyUserWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton> CreateRoomButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton> JoinRoomButton;

	UFUNCTION()
	virtual void OnCreateRoomButtonClicked();

	UFUNCTION()
	virtual void OnJoinRoomButtonClicked();

public:
	UFUNCTION()
	void UpdateEnabled(bool bEnabled);
};
