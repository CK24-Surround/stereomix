// Copyright Surround, Inc. All Rights Reserved.


#include "SMGameState.h"

#include "GameFramework/GameMode.h"
#include "Actors/Projectiles/Pool/SMProjectilePoolManagerComponent.h"
#include "Components/Core/SMRoundTimerManagerComponent.h"
#include "Components/Core/SMScoreMusicManagerComponent.h"
#include "Components/Core/SMTileManagerComponent.h"
#include "Utilities/SMLog.h"

ASMGameState::ASMGameState()
{
	RoundTimerManager = CreateDefaultSubobject<USMRoundTimerManagerComponent>(TEXT("RoundTimer"));
	TileManager = CreateDefaultSubobject<USMTileManagerComponent>(TEXT("TileManager"));
	ScoreMusicManager = CreateDefaultSubobject<USMScoreMusicManagerComponent>(TEXT("ScoreMusicManager"));
	ProjectilePoolManager = CreateDefaultSubobject<USMProjectilePoolManagerComponent>(TEXT("ProjectilePoolManager"));
}

void ASMGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void ASMGameState::HandleMatchIsWaitingToStart()
{
	Super::HandleMatchIsWaitingToStart();

	if (HasAuthority())
	{
		RoundTimerManager->OnPreRoundTimeExpired.AddDynamic(this, &ThisClass::OnPreRoundTimeExpiredCallback);
		RoundTimerManager->OnRoundTimeExpired.AddDynamic(this, &ThisClass::OnRoundTimeExpiredCallback);
		RoundTimerManager->OnPostRoundTimeExpired.AddDynamic(this, &ThisClass::OnPostRoundTimeExpiredCallback);
		RoundTimerManager->StartTimer();
	}

	if (GetNetMode() == NM_Client)
	{
		ScoreMusicManager->PlayScoreMusic();
	}
}

void ASMGameState::OnPreRoundTimeExpiredCallback()
{
	const UWorld* World = GetWorld();
	if (AGameMode* GameMode = World ? World->GetAuthGameMode<AGameMode>() : nullptr)
	{
		GameMode->StartMatch();
	}
}

void ASMGameState::OnRoundTimeExpiredCallback()
{
	const UWorld* World = GetWorld();
	if (AGameMode* GameMode = World ? World->GetAuthGameMode<AGameMode>() : nullptr)
	{
		GameMode->EndMatch();
	}

	TileManager->ShowGameResult();
}

void ASMGameState::OnPostRoundTimeExpiredCallback()
{
	const UWorld* World = GetWorld();
	if (AGameMode* GameMode = World ? World->GetAuthGameMode<AGameMode>() : nullptr)
	{
		GameMode->StartToLeaveMap();
	}
}
