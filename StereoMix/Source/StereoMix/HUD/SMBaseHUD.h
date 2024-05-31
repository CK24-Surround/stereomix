// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MVVMSubsystem.h"
#include "GameFramework/HUD.h"
#include "UI/ViewModel/SMViewModel.h"
#include "View/MVVMView.h"
#include "SMBaseHUD.generated.h"

class USMViewModel;

/**
 * 
 */
UCLASS()
class STEREOMIX_API ASMBaseHUD : public AHUD
{
	GENERATED_BODY()

public:
	ASMBaseHUD();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UUserWidget> MainWidgetClass;

	UPROPERTY(Transient)
	TObjectPtr<UUserWidget> MainWidget;

	/**
	 * 위젯을 생성하고 뷰포트에 추가합니다.
	 * @param WidgetClass 위젯 클래스
	 * @param ZOrder
	 * @return 생성된 위젯
	 */
	virtual UUserWidget* AddWidgetToViewport(const TSubclassOf<UUserWidget>& WidgetClass, int32 ZOrder = 0);

	/**
	 * 위젯에서 뷰모델을 가져옵니다.
	 * @tparam ViewModelT 뷰모델 타입
	 * @param Widget 뷰모델을 가져올 위젯
	 * @param ViewModelName 뷰모델 이름
	 * @return 위젯에서 가져온 뷰모델. 없을 경우 nullptr을 반환합니다.
	 */
	template <class ViewModelT>
	ViewModelT* GetViewModelFromWidget(const UUserWidget* Widget, const FName ViewModelName)
	{
		if (const UMVVMView* View = UMVVMSubsystem::GetViewFromUserWidget(Widget))
		{
			return Cast<ViewModelT>(View->GetViewModel(ViewModelName).GetInterface());
		}
		return nullptr;
	}

	static bool AddViewModelToWidget(const UUserWidget* Widget, const FName ViewModelName, const TScriptInterface<INotifyFieldValueChanged>& ViewModel)
	{
		if (UMVVMView* View = UMVVMSubsystem::GetViewFromUserWidget(Widget))
		{
			return View->SetViewModel(ViewModelName, ViewModel);
		}
		return false;
	}
};
