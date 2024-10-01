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

	SubLevels.Add(nullptr);
	SubLevels.Add(nullptr);
}

void ASMLevelChanger::PreEditChange(FProperty* PropertyAboutToChange)
{
	Super::PreEditChange(PropertyAboutToChange);

	if (PropertyAboutToChange && PropertyAboutToChange->GetFName() == GET_MEMBER_NAME_CHECKED(ASMLevelChanger, ActiveLevelCount))
	{
		if (ActiveLevelCount > SubLevels.Num())
		{
			for (int32 Index = SubLevels.Num(); Index < ActiveLevelCount; ++Index)
			{
				SubLevels.Add(nullptr);
			}
		}
	}
}

void ASMLevelChanger::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	if (HasAuthority() && SubLevels.Num() > 0)
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
	TArray<TObjectPtr<UWorld>> AvailableSubLevels;

	for (TObjectPtr<UWorld> SubLevel : SubLevels)
	{
		if (!IsLevelActive(SubLevel))
		{
			AvailableSubLevels.Add(SubLevel);
		}
	}

	if (AvailableSubLevels.Num() > 0)
	{
		TObjectPtr<UWorld> RandomSubLevel = AvailableSubLevels[FMath::RandRange(0, AvailableSubLevels.Num() - 1)];
		SetLevelVisibility(RandomSubLevel, true);
		MarkLevelAsActive(RandomSubLevel);
	}
}

bool ASMLevelChanger::IsLevelActive(const TObjectPtr<UWorld>& SubLevel) const
{
	return ActiveSubLevels.Contains(SubLevel);
}

void ASMLevelChanger::MarkLevelAsActive(const TObjectPtr<UWorld>& SubLevel)
{
	if (!ActiveSubLevels.Contains(SubLevel))
	{
		ActiveSubLevels.Add(SubLevel);
	}
}

void ASMLevelChanger::SetLevelVisibility(const TObjectPtr<UWorld>& SubLevel, bool bVisibility)
{
	UWorld* World = GetWorld();
	if (!HasAuthority() || !SubLevel || !World)
	{
		return;
	}

	TWeakObjectPtr<ASMLevelChanger> ThisWeakPtr(this);

	if (bVisibility)
	{
		UGameplayStatics::LoadStreamLevel(this, SubLevel->GetFName(), true, true, FLatentActionInfo());

		FTimerHandle TimerHandle;
		World->GetTimerManager().SetTimer(TimerHandle, [ThisWeakPtr, SubLevel] {
			if (ThisWeakPtr.Get())
			{
				ThisWeakPtr->SetLevelVisibility(SubLevel, false);
			}
		}, (SwitchInterval * ActiveLevelCount) + SwitchOffset, false);

		return;
	}

	float TimeToUnload = 0.0f;

	for (AActor* Actor : TActorRange<ASMObstacleBase>(World))
	{
		ASMObstacleBase* Obstacle = Cast<ASMObstacleBase>(Actor);
		ULevel* ObstacleLevel = Obstacle ? Obstacle->GetLevel() : nullptr;
		UObject* ObstacleOuter = ObstacleLevel ? ObstacleLevel->GetOuter() : nullptr;
		FString ObstacleLevelName = GetNameSafe(ObstacleOuter);
		if (Obstacle && ObstacleLevelName == GetNameSafe(SubLevel))
		{
			TimeToUnload = FMath::Max(TimeToUnload, Obstacle->GetDestroyEffectDuration());
			Obstacle->UnloadObstacle();
		}
	}

	MulticastShowActiveEffect();

	FTimerHandle TimerHandle;
	World->GetTimerManager().SetTimer(TimerHandle, [ThisWeakPtr, SubLevel] {
		if (ThisWeakPtr.Get())
		{
			UGameplayStatics::UnloadStreamLevel(ThisWeakPtr.Get(), GetFNameSafe(SubLevel), FLatentActionInfo(), true);
			ThisWeakPtr->ActiveSubLevels.Remove(SubLevel);
		}
	}, TimeToUnload, false);
}
