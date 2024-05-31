// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMBaseHUD.h"
#include "TurboLinkGrpcClient.h"
#include "TurboLinkGrpcService.h"
#include "SMTitleHUD.generated.h"

class USMGuestLoginViewModel;
class USMTitleViewModel;

/**
 * 
 */
UCLASS()
class STEREOMIX_API ASMTitleHUD : public ASMBaseHUD
{
	GENERATED_BODY()

public:
	ASMTitleHUD();

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UUserWidget> GuestLoginWidgetClass;

	UPROPERTY(Transient)
	TObjectPtr<USMTitleViewModel> TitleViewModel;

	UPROPERTY(Transient)
	TObjectPtr<UUserWidget> GuestLoginWidget;

	// TODO: 추후에 새로운 로그인 방식을 추가할 경우 LoginViewModel로 통일 후 LoginViewModel로 전부 처리하기

	UPROPERTY(Transient)
	TObjectPtr<USMGuestLoginViewModel> GuestLoginViewModel;

	void CreateGuestLoginWidget();
	
	UFUNCTION()
	virtual void OnAuthServiceStateChanged(EGrpcServiceState ChangedState);

	UFUNCTION()
	virtual void OnGuestLoginSubmit();

	UFUNCTION()
	virtual void OnLogin(EGrpcResultCode Result);

	UFUNCTION()
	virtual void GoToMainMenu();
};
