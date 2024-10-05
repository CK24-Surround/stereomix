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

#if WITH_EDITOR
void ASMLevelChanger::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (FProperty* PropertyThatChanged = PropertyChangedEvent.Property)
	{
		FName PropertyName = PropertyThatChanged->GetFName();

		if (PropertyName == GET_MEMBER_NAME_CHECKED(ThisClass, ActiveLevelCount))
		{
			if (ActiveLevelCount + 1 > SubLevels.Num())
			{
				for (int32 Index = SubLevels.Num(); Index < ActiveLevelCount + 1; ++Index)
				{
					SubLevels.Add(nullptr);
				}
			}
		}

		if (PropertyName == GET_MEMBER_NAME_CHECKED(ThisClass, LevelSwitchInterval) || PropertyName == GET_MEMBER_NAME_CHECKED(ThisClass, ObstacleSpawnEffectDuration))
		{
			const float ObstacleLoadAndUnloadDuration = ObstacleSpawnEffectDuration + 2.0f;
			if (LevelSwitchInterval <= ObstacleLoadAndUnloadDuration) // 인터벌을 안전하게 스폰과 제거에 소요되는 이펙트 재생시간보다 크게 설정합니다.
			{
				LevelSwitchInterval = ObstacleLoadAndUnloadDuration;
			}
		}
	}
}
#endif

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
		}, LevelSwitchInterval, true);
	}
}

void ASMLevelChanger::MulticastShowActiveEffect_Implementation()
{
	NiagaraComponent->Activate(true);
}

void ASMLevelChanger::SetRandomSubLevel()
{
	TArray<UWorld*> AvailableSubLevels;

	for (UWorld* SubLevel : SubLevels)
	{
		if (!IsLevelActive(SubLevel))
		{
			AvailableSubLevels.Add(SubLevel);
		}
	}

	if (AvailableSubLevels.Num() > 0)
	{
		const UWorld* RandomSubLevel = AvailableSubLevels[FMath::RandRange(0, AvailableSubLevels.Num() - 1)];
		SetLevelVisibility(RandomSubLevel, true);
		MarkLevelAsActive(RandomSubLevel);
	}
}

bool ASMLevelChanger::IsLevelActive(const UWorld* SubLevel) const
{
	return ActiveSubLevels.Contains(SubLevel);
}

void ASMLevelChanger::MarkLevelAsActive(const UWorld* SubLevel)
{
	ActiveSubLevels.AddUnique(SubLevel);
}

void ASMLevelChanger::SetLevelVisibility(const UWorld* SubLevel, bool bVisibility)
{
	UWorld* World = GetWorld();
	if (!HasAuthority() || !SubLevel || !World)
	{
		return;
	}

	TWeakObjectPtr<ASMLevelChanger> ThisWeakPtr(this);

	if (bVisibility)
	{
		UGameplayStatics::LoadStreamLevel(this, SubLevel->GetFName(), true, false, FLatentActionInfo());

		FTimerHandle TimerHandle;
		World->GetTimerManager().SetTimer(TimerHandle, [ThisWeakPtr, SubLevel] {
			if (ThisWeakPtr.Get())
			{
				ThisWeakPtr->SetLevelVisibility(SubLevel, false);
			}
		}, (LevelSwitchInterval * ActiveLevelCount) + ObstacleSpawnEffectDuration, false);
	}
	else
	{
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
}
