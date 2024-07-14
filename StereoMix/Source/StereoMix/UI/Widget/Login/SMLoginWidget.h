// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Backend/Client/SMClientAuthSubsystem.h"
#include "SMGuestLoginPopup.h"
#include "SGame/GameMessage.h"
#include "UI/Widget/Frontend/SMFrontendElementWidget.h"
#include "UI/Widget/Popup/SMAlertPopup.h"

#include "SMLoginWidget.generated.h"

class UCommonTextBlock;
class USMGrpcGameSubsystem;
class UGameServiceClient;
class UGameService;

UENUM(BlueprintType)
enum class ELoginUiState : uint8
{
	Idle,
	Connecting,
	VersionCheck,
	Login,
	ProcessFailed
};

/**
 *
 */
UCLASS(Abstract)
class STEREOMIX_API USMLoginWidget : public USMFrontendElementWidget
{
	GENERATED_BODY()

public:
	USMLoginWidget();

protected:
	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	virtual void NativeOnActivated() override;

	virtual void NativeOnDeactivated() override;

	UFUNCTION(BlueprintCallable)
	void SetUiState(ELoginUiState NewState);

	UFUNCTION()
	void OnGameServiceStateChanged(EGrpcServiceState ServiceState);

	UFUNCTION()
	void OnGetServiceVersionResponse(FGrpcContextHandle Handle, const FGrpcResult& GrpcResult, const FGrpcGameGetServiceVersionResponse& Response);

	UFUNCTION()
	void OnAuthServiceStateChanged(EGrpcServiceState ServiceState);

	UFUNCTION()
	void OnLoginResponse(ELoginResult Result);

	UFUNCTION()
	void OnSubmitGuestLogin(USMPopup* Popup);

	UPROPERTY()
	TObjectPtr<USMClientAuthSubsystem> GrpcAuthSubsystem;

	UPROPERTY()
	TObjectPtr<USMGrpcGameSubsystem> GrpcGameSubsystem;

	UPROPERTY()
	TObjectPtr<UGameService> GameService;

	UPROPERTY()
	TObjectPtr<UGameServiceClient> GameServiceClient;

	FGrpcContextHandle GrpcGetServiceVersionContextHandle;

private:
	// ======================================================================
	// Components
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess, BindWidget))
	TObjectPtr<UCommonTextBlock> ProgressTextBlock;

	// ======================================================================
	// Frontend Elements

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Frontend Elements", meta = (AllowPrivateAccess))
	TSubclassOf<USMGuestLoginPopup> GuestLoginPopupClass;


	// ======================================================================
	// Properties
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, meta = (AllowPrivateAccess))
	ELoginUiState UiState = ELoginUiState::Idle;
};
