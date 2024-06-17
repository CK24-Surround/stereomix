// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FMODBlueprintStatics.h"
#include "Data/SMTeam.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SMBackgroundMusicSubsystem.generated.h"

UENUM(BlueprintType)
enum class ETeamChoiceMusicParameterType : uint8
{
	None = 0,
	FutureBass = 1,
	EDM = 2,
};

/**
 * Background Music Player Subsystem
 */
UCLASS()
class STEREOMIX_API USMBackgroundMusicSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	USMBackgroundMusicSubsystem();

	virtual void Deinitialize() override;

	/** BGM을 재생합니다. 이미 재생 중인 경우 Team에 맞는 음악으로 변경합니다. */
	void PlayTeamBackgroundMusic(ESMTeam Team);

	/** BGM을 중지하고 이벤트 인스턴스를 해제합니다. */
	void StopAndReleaseBackgroundMusic() const;

	bool IsPlaying() const;

protected:
	UPROPERTY(EditDefaultsOnly, Category="Background Music")
	TObjectPtr<UFMODEvent> BackgroundMusic;

private:
	FFMODEventInstance TeamBackgroundMusicInstance = { nullptr };
};
