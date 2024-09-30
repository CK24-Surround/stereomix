// Copyright Studio Surround. All Rights Reserved.

#include "SMLevelChanger.h"

#include "Engine/LevelStreaming.h"
#include "EngineUtils.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Obstacle/SMObstacleBase.h"
#include "Utilities/SMCollision.h"
#include "Utilities/SMLog.h"


ASMLevelChanger::ASMLevelChanger()
{
	bReplicates = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;

	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	NiagaraComponent->SetupAttachment(RootComponent);
	NiagaraComponent->SetCollisionProfileName(SMCollisionProfileName::NoCollision);
	NiagaraComponent->SetAutoActivate(false);
}

void ASMLevelChanger::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	if (HasAuthority())
	{
		SetRandomSubLevel();

		FTimerHandle TimerHandle;
		TWeakObjectPtr<ASMLevelChanger> ThisWeakPtr(this);
		World->GetTimerManager().SetTimer(TimerHandle, [ThisWeakPtr] {
			if (ThisWeakPtr.Get())
			{
				ThisWeakPtr->SetRandomSubLevel();
			}
		}, SwitchInterval, true);
	}
}

void ASMLevelChanger::MulticastShowActiveEffect_Implementation()
{
	NiagaraComponent->Activate(true);
}

void ASMLevelChanger::SetRandomSubLevel()
{
	TObjectPtr<UWorld> RandomSubLevel = SubLevels[FMath::RandRange(0, SubLevels.Num() - 1)];
	if (CurrentSubLevel != RandomSubLevel->GetFName())
	{
		CurrentSubLevel = RandomSubLevel->GetFName();
		SetLevelVisibility(CurrentSubLevel, true);
		return;
	}

	if (SubLevels.Num() > 1)
	{
		SetRandomSubLevel();
	}
}

void ASMLevelChanger::SetLevelVisibility(FName LevelName, bool bVisibility)
{
	UWorld* World = GetWorld();
	if (!HasAuthority() || LevelName.IsNone() || !World)
	{
		return;
	}

	TWeakObjectPtr<ASMLevelChanger> ThisWeakPtr = this;

	if (bVisibility)
	{
		UGameplayStatics::LoadStreamLevel(this, LevelName, true, true, FLatentActionInfo());

		FTimerHandle TimerHandle;
		World->GetTimerManager().SetTimer(TimerHandle, [ThisWeakPtr, LevelName] {
			if (ThisWeakPtr.Get())
			{
				ThisWeakPtr->SetLevelVisibility(LevelName, false);
			}
		}, LevelLifetime, false);

		return;
	}

	float TimeToUnload = 0.0f;

	for (AActor* Actor : TActorRange<ASMObstacleBase>(World))
	{
		ASMObstacleBase* Obstacle = Cast<ASMObstacleBase>(Actor);
		ULevel* Level = Obstacle ? Obstacle->GetLevel() : nullptr;
		UObject* Outer = Level ? Level->GetOuter() : nullptr;
		FString ObstacleLevelName = GetNameSafe(Outer);
		if (Obstacle && ObstacleLevelName == LevelName)
		{
			TimeToUnload = FMath::Max(TimeToUnload, Obstacle->GetDestroyEffectDuration());
			Obstacle->UnloadObstacle();
		}
	}

	MulticastShowActiveEffect();

	FTimerHandle TimerHandle;
	World->GetTimerManager().SetTimer(TimerHandle, [ThisWeakPtr, LevelName] {
		if (ThisWeakPtr.Get())
		{
			UGameplayStatics::UnloadStreamLevel(ThisWeakPtr.Get(), LevelName, FLatentActionInfo(), true);
		}
	}, TimeToUnload, false);
}
