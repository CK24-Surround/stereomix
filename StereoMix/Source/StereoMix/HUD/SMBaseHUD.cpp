// Copyright Surround, Inc. All Rights Reserved.


#include "SMBaseHUD.h"

#include "Blueprint/UserWidget.h"

ASMBaseHUD::ASMBaseHUD()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ASMBaseHUD::BeginPlay()
{
	Super::BeginPlay();

	checkfSlow(MainWidgetClass, TEXT("MainWidgetClass is not set."))
	MainWidget = AddWidgetToViewport(MainWidgetClass);
}

void ASMBaseHUD::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

UUserWidget* ASMBaseHUD::AddWidgetToViewport(const TSubclassOf<UUserWidget>& WidgetClass, int32 ZOrder)
{
	UUserWidget* Widget = CreateWidget<UUserWidget>(GetOwningPlayerController(), WidgetClass);
	Widget->AddToViewport(ZOrder);
	return Widget;
}
