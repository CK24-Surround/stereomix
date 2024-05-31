// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMViewModel.h"
#include "Backend/Client/SMClientAuthSubsystem.h"
#include "SMTitleViewModel.generated.h"

UENUM(BlueprintType)
enum class ETitleUiState : uint8
{
	Idle,
	Connecting,
	Connected,
	ConnectionFailed,
	RequestingLogin,
	LoginSuccess,
	LoginFailed
};

/**
 * Title ViewModel
 */
UCLASS(BlueprintType)
class STEREOMIX_API USMTitleViewModel : public USMViewModel
{
	GENERATED_BODY()


public:
	USMTitleViewModel();

	virtual void InitializeViewModel(UWorld* InWorld) override;

	virtual void BeginDestroy() override;

	ETitleUiState GetUiState() const { return UiState; }

protected:
	void SetUiState(ETitleUiState NewState);
	
private:
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter=SetUiState, Getter=GetUiState, meta=(AllowPrivateAccess))
	ETitleUiState UiState;

	UPROPERTY()
	TWeakObjectPtr<USMClientAuthSubsystem> AuthSubsystem;
	
	UFUNCTION()
	void OnServiceStateChanged(EGrpcServiceState ServiceState);
	
	UFUNCTION()
	void OnLoginResponse(ELoginResult Result);

	UFUNCTION()
	void GoToMainMenu();
};
