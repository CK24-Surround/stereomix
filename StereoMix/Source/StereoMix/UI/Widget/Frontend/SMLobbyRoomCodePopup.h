// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/EditableTextBox.h"
#include "UI/Widget/Popup/SMPopup.h"
#include "SMLobbyRoomCodePopup.generated.h"

/**
 * StereoMix Lobby Room Code Popup Widget
 */
UCLASS()
class STEREOMIX_API USMLobbyRoomCodePopup : public USMPopup
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GuestLoginPopup")
	int32 RoomCodeDesiredLength = 6;
	
	USMLobbyRoomCodePopup();

	UFUNCTION()
	FText GetRoomCode() const { return RoomCodeInputBox->GetText(); }

protected:
	
	virtual void NativeConstruct() override;

	virtual void NativeOnActivated() override;

	virtual bool CanSubmit() override;

	virtual void PerformSubmit() override;

	UFUNCTION()
	virtual void OnRoomCodeTextChanged(const FText& ChangedRoomCode);

	bool IsValidRoomCode(const FString& RoomCode) const;

private:
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess, BindWidget))
	TObjectPtr<UEditableTextBox> RoomCodeInputBox;
};
