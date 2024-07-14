// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Backend/Client/SMClientAuthSubsystem.h"
#include "Components/Button.h"
#include "SMFrontendElementViewModel.h"

#include "SMLoginViewModel.generated.h"

UENUM(BlueprintType)
enum class ELoginViewModelUiState : uint8
{
	Idle,
	ReadyToSubmit,
	Submit,
	Requesting,
	LoginSuccess,
	LoginFailed
};

/**
 * Default Login ViewModel
 */
UCLASS(BlueprintType)
class STEREOMIX_API USMLoginViewModel : public USMFrontendElementViewModel
{
	GENERATED_BODY()

public:
	USMLoginViewModel();

	virtual void BeginDestroy() override;

	ELoginViewModelUiState GetUiState() const { return UiState; }

protected:
	TWeakObjectPtr<USMClientAuthSubsystem> AuthSubsystem;

	void SetUiState(ELoginViewModelUiState NewState);

	virtual void OnSubmit();

private:
	UPROPERTY(BlueprintReadWrite, FieldNotify, meta = (AllowPrivateAccess))
	ELoginViewModelUiState UiState;
};
