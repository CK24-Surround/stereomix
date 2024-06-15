// Copyright Surround, Inc. All Rights Reserved.


#include "SMCommonButton.h"

#include "FMODBlueprintStatics.h"
#include "UI/Style/SMButtonStyle.h"

void USMCommonButton::HandleFocusReceived()
{
	Super::HandleFocusReceived();

	// if (const USMButtonStyle* ButtonStyle = Cast<USMButtonStyle>(GetStyleCDO()); ButtonStyle && ButtonStyle->HoverSound)
	// {
	// 	UFMODBlueprintStatics::PlayEvent2D(this, ButtonStyle->HoverSound, true);
	// }
	// else if (HoverSound)
	// {
	// 	UFMODBlueprintStatics::PlayEvent2D(this, HoverSound, true);
	// }
}

void USMCommonButton::NativeOnHovered()
{
	Super::NativeOnHovered();

	if (const USMButtonStyle* ButtonStyle = Cast<USMButtonStyle>(GetStyleCDO()); ButtonStyle && ButtonStyle->HoverSound)
	{
		UFMODBlueprintStatics::PlayEvent2D(this, ButtonStyle->HoverSound, true);
	}
	else if (HoverSound)
	{
		UFMODBlueprintStatics::PlayEvent2D(this, HoverSound, true);
	}
}

void USMCommonButton::NativeOnClicked()
{
	Super::NativeOnClicked();

	if (const USMButtonStyle* ButtonStyle = Cast<USMButtonStyle>(GetStyleCDO()); ButtonStyle && ButtonStyle->ClickSound)
	{
		UFMODBlueprintStatics::PlayEvent2D(this, ButtonStyle->ClickSound, true);
	}
	else if (ClickSound)
	{
		UFMODBlueprintStatics::PlayEvent2D(this, ClickSound, true);
	}
}
