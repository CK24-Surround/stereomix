// Copyright Surround, Inc. All Rights Reserved.


#include "SMUserWidget_HUD.h"

#include "SMUserWidget_Scoreboard.h"

void USMUserWidget_HUD::SetASC(UAbilitySystemComponent* InASC)
{
	Super::SetASC(InASC);

	Scoreboard->SetASC(InASC);
}
