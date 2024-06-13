// Copyright Surround, Inc. All Rights Reserved.


#include "SMPlayerController.h"

#include "HUD/SMClientHUD.h"

ASMPlayerController::ASMPlayerController()
{
}

void ASMPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (PlayerState != nullptr)
	{
		if (AHUD* HUD = GetHUD())
		{
			if (ASMClientHUD* ClientHUD = Cast<ASMClientHUD>(HUD))
			{
				ClientHUD->InitPlayerState(PlayerState);
			}
		}
	}
}
