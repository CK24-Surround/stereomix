// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FMODBlueprintStatics.h"
#include "Components/ActorComponent.h"
#include "Data/SMTeam.h"
#include "SMScoreMusicManagerComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STEREOMIX_API USMScoreMusicManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USMScoreMusicManagerComponent();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void PlayScoreMusic();

	ESMTeam GetScoreMusicPlayingTeam() const { return ScoreMusicPlayingTeam; }

	void SetScoreMusicPlayingTeam(ESMTeam NewTeam);

protected:
	UFUNCTION()
	void OnRep_ScoreMusicPlayingTeam();

	static constexpr float ScoreMusicParameterNone = 0.0f;

	static constexpr float ScoreMusicParameterEDM = 2.0f;

	static constexpr float ScoreMusicParameterFB = 1.0f;

	static const FName ScoreMusicParameterName;

	UPROPERTY(EditAnywhere, Category = "Design")
	TObjectPtr<UFMODEvent> ScoreMusic;

	FFMODEventInstance ScoreMusicEventInstance;

	UPROPERTY(ReplicatedUsing = "OnRep_ScoreMusicPlayingTeam")
	ESMTeam ScoreMusicPlayingTeam = ESMTeam::None;
};
