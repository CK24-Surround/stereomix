// Copyright Surround, Inc. All Rights Reserved.


#include "SMUserWidget_HUD.h"

void USMUserWidget_HUD::SetASC(UAbilitySystemComponent* InASC)
{
	Super::SetASC(InASC);

	USMUserWidget_GameHUD* GameHUD = GameStack->AddWidget<USMUserWidget_GameHUD>(GameHUDClass);
	GameHUD->SetASC(InASC);
}
