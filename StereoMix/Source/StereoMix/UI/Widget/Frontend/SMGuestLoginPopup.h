// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/EditableTextBox.h"
#include "UI/Widget/Popup/SMPopup.h"
#include "SMGuestLoginPopup.generated.h"

/**
 * StereoMix Guest Login Popup Widget
 */
UCLASS()
class STEREOMIX_API USMGuestLoginPopup : public USMPopup
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GuestLoginPopup")
	int32 MaxUserNameLength = 10;

	USMGuestLoginPopup();

	FText GetUserName() const { return UserNameInputBox->GetText(); }

protected:
	virtual void NativeConstruct() override;

	virtual void NativeOnActivated() override;

	virtual bool CanSubmit() override;

	virtual void PerformSubmit() override;

	UFUNCTION()
	virtual void OnUserNameTextChanged(const FText& ChangedUserName);

	bool IsValidUserName(const FString& UserName) const;

private:
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess, BindWidget))
	TObjectPtr<UEditableTextBox> UserNameInputBox;
};
