// Copyright Surround, Inc. All Rights Reserved.


#include "SMButtonBase.h"

#include "FMODBlueprintStatics.h"
#include "UI/Style/SMButtonStyle.h"

void USMButtonBase::NativeOnHovered()
{
	Super::NativeOnHovered();


	if (HoveredFMODSoundOverride)
	{
		UFMODBlueprintStatics::PlayEvent2D(this, HoveredFMODSoundOverride, true);
	}
	else if (const USMButtonStyle* ButtonStyle = Cast<USMButtonStyle>(GetStyleCDO()); ButtonStyle && ButtonStyle->HoveredFMODSound)
	{
		UFMODBlueprintStatics::PlayEvent2D(this, ButtonStyle->HoveredFMODSound, true);
	}
}

void USMButtonBase::NativeOnSelected(bool bBroadcast)
{
	Super::NativeOnSelected(bBroadcast);

	if (SelectedFMODSoundOverride)
	{
		UFMODBlueprintStatics::PlayEvent2D(this, SelectedFMODSoundOverride, true);
	}
	else if (const USMButtonStyle* ButtonStyle = Cast<USMButtonStyle>(GetStyleCDO()); ButtonStyle && ButtonStyle->SelectedFMODSound)
	{
		UFMODBlueprintStatics::PlayEvent2D(this, ButtonStyle->SelectedFMODSound, true);
	}
}

void USMButtonBase::NativeOnPressed()
{
	Super::NativeOnPressed();

	if (!GetLocked())
	{
		if (PressedFMODSoundOverride)
		{
			UFMODBlueprintStatics::PlayEvent2D(this, PressedFMODSoundOverride, true);
		}
		else if (const USMButtonStyle* ButtonStyle = Cast<USMButtonStyle>(GetStyleCDO()); ButtonStyle && ButtonStyle->PressedFMODSound)
		{
			UFMODBlueprintStatics::PlayEvent2D(this, ButtonStyle->PressedFMODSound, true);
		}
	}
}
