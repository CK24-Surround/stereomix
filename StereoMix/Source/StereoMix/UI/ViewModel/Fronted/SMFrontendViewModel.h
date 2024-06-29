// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMFrontendElementViewModel.h"
#include "UI/ViewModel/SMViewModel.h"
#include "UObject/Object.h"

#include "SMFrontendViewModel.generated.h"

class ASMFrontendPlayerController;

UENUM(BlueprintType)
enum class EFrontendUiState : uint8
{
	Login,
	MainMenu,
	Settings,
	Lobby
};

/**
 * StereoMix Frontend ViewModel
 */
UCLASS(meta = (MVVMAllowedContextCreationType = "Manual|CreateInstance"))
class STEREOMIX_API USMFrontendViewModel : public USMViewModel
{
	GENERATED_BODY()

public:
	USMFrontendViewModel();

	virtual void InitViewModel(APlayerController* PlayerController) override;

	EFrontendUiState GetUiState() const { return UiState; }

	void SetUiState(EFrontendUiState NewUiState);

	USMFrontendElementViewModel* GetCurrentElementViewModel() const { return CurrentElementViewModel.Get(); }

	ASMFrontendPlayerController* GetFrontendPlayerController() const { return FrontendPlayerController.Get(); }

	/**
	 *	배경색을 변경합니다.
	 * @param NewColor 변경할 색상
	 */
	void SetBackgroundColor(FLinearColor NewColor);

protected:
	void SetCurrentElementViewModel(USMFrontendElementViewModel* NewElementViewModel);

private:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, FieldNotify, Setter, Getter, meta = (AllowPrivateAccess))
	EFrontendUiState UiState;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, FieldNotify, Setter, meta = (AllowPrivateAccess))
	TObjectPtr<USMFrontendElementViewModel> CurrentElementViewModel;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, FieldNotify, Setter, meta = (AllowPrivateAccess))
	FLinearColor BackgroundColor;

	TWeakObjectPtr<ASMFrontendPlayerController> FrontendPlayerController;
};
