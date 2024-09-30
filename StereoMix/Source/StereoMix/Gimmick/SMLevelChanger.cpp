// Copyright Studio Surround. All Rights Reserved.

#include "SMLevelChanger.h"

#include "Engine/LevelStreaming.h"
#include "Kismet/GameplayStatics.h"
#include "Obstacle/SMObstacleBase.h"
#include "Utilities/SMLog.h"


void ASMLevelChanger::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();

	if (!HasAuthority() || !World)
	{
		return;
	}

	// 시작할 때 첫 번째 서브 레벨을 활성화
	MulticastSetLevelVisibility(SubLevel1, true);

	TWeakObjectPtr<ASMLevelChanger> ThisWeakPtr = this;

	FTimerHandle PreSpawnEffectTimerHandle;
	World->GetTimerManager().SetTimer(PreSpawnEffectTimerHandle, [ThisWeakPtr] {
		if (ThisWeakPtr.Get())
		{
			ThisWeakPtr->SwitchLevels();
		}
	}, SwitchInterval, true);
}

void ASMLevelChanger::MulticastSetLevelVisibility_Implementation(FName LevelName, bool bVisible)
{
	SetLevelVisibility(LevelName, true);
}

void ASMLevelChanger::SwitchLevels()
{
	bIsSubLevel1Active = !bIsSubLevel1Active;
	MulticastSetLevelVisibility(bIsSubLevel1Active ? SubLevel1 : SubLevel2, true);
}

void ASMLevelChanger::SetLevelVisibility(FName LevelName, bool bVisibility)
{
	if (LevelName.IsNone())
	{
		return;
	}

	if (bVisibility)
	{
		if (UWorld* World = GetWorld())
		{
			UGameplayStatics::LoadStreamLevel(this, LevelName, true, true, FLatentActionInfo());

			FTimerHandle TimerHandle;
			World->GetTimerManager().SetTimer(TimerHandle, [this, LevelName] {
				SetLevelVisibility(LevelName, false);
			}, LevelLifetime, false);
		}

		return;
	}

	if (UWorld* World = GetWorld())
	{
		float TimeToUnload = 0.0f;

		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(World, ASMObstacleBase::StaticClass(), FoundActors);
		for (AActor* Actor : FoundActors)
		{
			if (Actor && Actor->GetLevel()->GetOuter()->GetFName() == LevelName)
			{
				if (ASMObstacleBase* Obstacle = Cast<ASMObstacleBase>(Actor))
				{
					TimeToUnload = FMath::Max(TimeToUnload, Obstacle->GetDestroyEffectDuration());
					Obstacle->UnloadObstacle();
				}
			}
		}

		FTimerHandle TimerHandle;
		World->GetTimerManager().SetTimer(TimerHandle, [this, LevelName] {
			UGameplayStatics::UnloadStreamLevel(this, LevelName, FLatentActionInfo(), true);
		}, TimeToUnload, false);

		return;
	}

	UGameplayStatics::UnloadStreamLevel(this, LevelName, FLatentActionInfo(), true);
}
