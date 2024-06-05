// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMFrontendElementWidget.h"
#include "SMFrontendWidgetStack.h"
#include "SMGuestLoginPopup.h"
#include "Backend/Client/SMClientAuthSubsystem.h"
#include "SMLoginWidget.generated.h"

/**
 * 
 */
UCLASS()
class STEREOMIX_API USMLoginWidget : public USMFrontendElementWidget
{
	GENERATED_BODY()

public:
	USMLoginWidget();
	
protected:
	UPROPERTY()
	TObjectPtr<USMClientAuthSubsystem> AuthSubsystem;

	virtual void NativeConstruct() override;

	virtual void NativeOnActivated() override;

	virtual void NativeOnDeactivated() override;
	
	UFUNCTION()
	void OnAuthServiceStateChanged(EGrpcServiceState ServiceState);
	
	UFUNCTION()
	void OnLoginResponse(ELoginResult Result);
	
	UFUNCTION()
	void OnSubmitGuestLogin();

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Frontend Elements", meta=(AllowPrivateAccess))
	TSubclassOf<USMGuestLoginPopup> GuestLoginPopupClass;
	
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Components", meta=(AllowPrivateAccess, BindWidget))
	TObjectPtr<UCommonActivatableWidgetStack> LoginPopupStack;

	UPROPERTY(Transient, BlueprintReadOnly, Category="Login Popup", meta=(AllowPrivateAccess))
	TObjectPtr<USMGuestLoginPopup> GuestLoginPopup;
};
