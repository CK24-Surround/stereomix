// Copyright Studio Surround. All Rights Reserved.


#include "SMScoreMusicManagerComponent.h"

#include "FMODBlueprintStatics.h"
#include "Net/UnrealNetwork.h"
#include "Utilities/SMLog.h"

const FName USMScoreMusicManagerComponent::ScoreMusicParameterName = TEXT("Winner");

USMScoreMusicManagerComponent::USMScoreMusicManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
}

void USMScoreMusicManagerComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, ScoreMusicPlayingTeam);
}

void USMScoreMusicManagerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	UFMODBlueprintStatics::EventInstanceStop(ScoreMusicEventInstance, true);
}

void USMScoreMusicManagerComponent::PlayScoreMusic()
{
	ScoreMusicEventInstance = UFMODBlueprintStatics::PlayEvent2D(this, ScoreMusic, true);
	UFMODBlueprintStatics::SetGlobalParameterByName(ScoreMusicParameterName, ScoreMusicParameterNone);
}

void USMScoreMusicManagerComponent::SetScoreMusicPlayingTeam(ESMTeam NewTeam)
{
	ScoreMusicPlayingTeam = NewTeam;
}

void USMScoreMusicManagerComponent::OnRep_ScoreMusicPlayingTeam()
{
	if (ScoreMusicEventInstance.Instance->isValid())
	{
		NET_LOG(GetOwner(), Log, TEXT("%s로 ScoreMusic 변경"), *UEnum::GetValueAsString(ScoreMusicPlayingTeam))
		switch (ScoreMusicPlayingTeam)
		{
			case ESMTeam::None:
			{
				UFMODBlueprintStatics::SetGlobalParameterByName(ScoreMusicParameterName, ScoreMusicParameterNone);
				break;
			}
			case ESMTeam::EDM:
			{
				UFMODBlueprintStatics::SetGlobalParameterByName(ScoreMusicParameterName, ScoreMusicParameterEDM);
				break;
			}
			case ESMTeam::FutureBass:
			{
				UFMODBlueprintStatics::SetGlobalParameterByName(ScoreMusicParameterName, ScoreMusicParameterFB);
				break;
			}
			case ESMTeam::Max:
			{
				break;
			}
		}
	}
}
