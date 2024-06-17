// Copyright Surround, Inc. All Rights Reserved.


#include "SMBackgroundMusicSubsystem.h"

#include "FMODEvent.h"

USMBackgroundMusicSubsystem::USMBackgroundMusicSubsystem()
{
	static ConstructorHelpers::FObjectFinder<UFMODAsset> FMODTeamBackgroundMusicEvent(TEXT("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/BGM/TeamChoice.TeamChoice'"));
	
	BackgroundMusic = CastChecked<UFMODEvent>(FMODTeamBackgroundMusicEvent.Object);
}

void USMBackgroundMusicSubsystem::Deinitialize()
{
	Super::Deinitialize();
	UFMODBlueprintStatics::EventInstanceStop(TeamBackgroundMusicInstance, true);
}

void USMBackgroundMusicSubsystem::PlayTeamBackgroundMusic(const ESMTeam Team)
{
	if (!ensure(BackgroundMusic))
	{
		return;
	}

	if (!UFMODBlueprintStatics::EventInstanceIsValid(TeamBackgroundMusicInstance))
	{
		TeamBackgroundMusicInstance = UFMODBlueprintStatics::PlayEvent2D(this, BackgroundMusic, true);
	}

	float ParameterValue;
	switch (Team)
	{
		case ESMTeam::FutureBass:
			ParameterValue = static_cast<float>(ETeamChoiceMusicParameterType::FutureBass);
			break;

		case ESMTeam::EDM:
			ParameterValue = static_cast<float>(ETeamChoiceMusicParameterType::EDM);
			break;

		case ESMTeam::None:
		default:
			ParameterValue = static_cast<float>(ETeamChoiceMusicParameterType::None);
			break;
	}

	UFMODBlueprintStatics::SetGlobalParameterByName("Winner", ParameterValue);
}

void USMBackgroundMusicSubsystem::StopAndReleaseBackgroundMusic() const
{
	UFMODBlueprintStatics::EventInstanceStop(TeamBackgroundMusicInstance, true);
}

bool USMBackgroundMusicSubsystem::IsPlaying() const
{
	return UFMODBlueprintStatics::EventInstanceIsValid(TeamBackgroundMusicInstance);
}
