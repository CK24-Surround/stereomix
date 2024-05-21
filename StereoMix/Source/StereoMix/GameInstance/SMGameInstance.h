// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class STEREOMIX_API USMGameInstance : public UGameInstance
{
	GENERATED_BODY()

protected:
	/**
	 * 로딩 스크린 위젯 인스턴스입니다.
	 */
	UPROPERTY(Transient)
	TObjectPtr<UUserWidget> LoadingScreenWidget;

public:
	USMGameInstance();

	/**
	 * 로딩 스크린 UI 위젯 클래스입니다. 런타임에 해당 클래스의 인스턴스가 생성됩니다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=UI)
	TSubclassOf<UUserWidget> LoadingScreenWidgetClass;

	virtual void Init() override;

	virtual void StartGameInstance() override;

	/**
	 * 로딩 스크린을 표시합니다.
	 */
	void ShowLoadingScreen();

	/**
	 * 로딩 스크린을 숨깁니다.
	 */
	void HideLoadingScreen();
};
