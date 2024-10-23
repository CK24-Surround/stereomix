// Copyright Studio Surround. All Rights Reserved.


#include "SMTutorialManagerComponent.h"

#include "GameFramework/GameModeBase.h"
#include "EngineUtils.h"
#include "LevelSequencePlayer.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "AbilitySystem/Abilities/Common/SMGA_Hold.h"
#include "AbilitySystem/Abilities/NoiseBreak/SMGA_NoiseBreak.h"
#include "AbilitySystem/Abilities/Skill/SMGA_Skill.h"
#include "AbilitySystem/AttributeSets/SMCharacterAttributeSet.h"
#include "Actors/Character/AI/SMAICharacterBase.h"
#include "Actors/Character/Player/SMPlayerCharacterBase.h"
#include "Actors/Tiles/SMTile.h"
#include "Actors/Tutorial/SMProgressTriggerBase.h"
#include "Actors/Tutorial/SMTrainingDummy.h"
#include "Actors/Tutorial/SMTutorialLocation.h"
#include "Actors/Tutorial/SMTutorialWall.h"
#include "Components/CapsuleComponent.h"
#include "Components/PlayerController/SMTutorialUIControlComponent.h"
#include "Data/DataAsset/Character/SMPlayerCharacterDataAsset.h"
#include "Data/DataTable/Tutorial/SMTutorialScript.h"
#include "FunctionLibraries/SMAbilitySystemBlueprintLibrary.h"
#include "Games/SMGamePlayerState.h"
#include "Utilities/SMLog.h"


USMTutorialManagerComponent::USMTutorialManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;
	bAutoActivate = true;
}

void USMTutorialManagerComponent::InitializeComponent()
{
	Super::InitializeComponent();

	auto HideWall = [](ASMTutorialWall* Wall) {
		Wall->SetActorHiddenInGame(true);
	};

	auto DeactivateWall = [](ASMTutorialWall* Wall) {
		Wall->SetActorHiddenInGame(true);
		Wall->SetActorEnableCollision(false);
	};

	for (ASMTutorialWall* TutorialWall : TActorRange<ASMTutorialWall>(GetWorld()))
	{
		if (!TutorialWall)
		{
			continue;
		}

		if (TutorialWall->ActorHasTag(TEXT("Sampling")))
		{
			SamplingWall = TutorialWall;
		}

		if (TutorialWall->ActorHasTag(TEXT("Hold")))
		{
			HoldWall = TutorialWall;
			HideWall(TutorialWall);
		}

		if (TutorialWall->ActorHasTag(TEXT("NoiseBreak")))
		{
			NoiseBreakWall = TutorialWall;
			HideWall(TutorialWall);
		}

		if (TutorialWall->ActorHasTag(TEXT("HealPack")))
		{
			HealPackWall = TutorialWall;
			HideWall(TutorialWall);
		}

		if (TutorialWall->ActorHasTag(TEXT("NextSection")))
		{
			NextSectionWall = TutorialWall;
			DeactivateWall(TutorialWall);
		}

		if (TutorialWall->ActorHasTag(TEXT("BattleEntrance")))
		{
			BattleEntranceWall = TutorialWall;
			DeactivateWall(TutorialWall);
		}

		if (TutorialWall->ActorHasTag(TEXT("BattleExit")))
		{
			BattleExitWall = TutorialWall;
		}
	}

	for (ASMProgressTriggerBase* ProgressTrigger : TActorRange<ASMProgressTriggerBase>(GetWorld()))
	{
		if (!ProgressTrigger)
		{
			continue;
		}

		if (ProgressTrigger->ActorHasTag(TEXT("MovePractice")))
		{
			MovePracticeTrigger = ProgressTrigger;
		}

		if (ProgressTrigger->ActorHasTag(TEXT("Next")))
		{
			NextTrigger = ProgressTrigger;
		}

		if (ProgressTrigger->ActorHasTag(TEXT("End")))
		{
			EndTrigger = ProgressTrigger;
		}

		if (ProgressTrigger->ActorHasTag(TEXT("BattleStart")))
		{
			BattleStartTrigger = ProgressTrigger;
		}
	}

	for (const auto& Tile : TActorRange<ASMTile>(GetWorld()))
	{
		if (!Tile)
		{
			continue;
		}

		if (Tile->ActorHasTag(TEXT("Sampling")))
		{
			SamplingTiles.Add(Tile);
		}

		if (Tile->ActorHasTag(TEXT("NoiseBreak")))
		{
			NoiseBreakTiles.Add(Tile);
		}

		if (Tile->ActorHasTag(TEXT("HealPack")))
		{
			HealPackTiles.Add(Tile);
		}

		if (Tile->ActorHasTag(TEXT("PreNextSection")))
		{
			PreNextSectionTiles.Add(Tile);
		}
	}

	for (const ASMTutorialLocation* Location : TActorRange<ASMTutorialLocation>(GetWorld()))
	{
		if (!Location)
		{
			continue;
		}

		if (Location->ActorHasTag(TEXT("NoiseBreakRestart")))
		{
			NoiseBreakRestartLocation = Location->GetActorLocation();
		}

		if (Location->ActorHasTag(TEXT("AISpawn")))
		{
			AISpawnLocation = Location->GetActorLocation();
		}

		if (Location->ActorHasTag(TEXT("LocationIndicator3X3")))
		{
			LocationIndicator3X3Component = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), LocationIndicator3X3, Location->GetActorLocation(), FRotator::ZeroRotator, FVector(1), false, false, ENCPoolMethod::ManualRelease);

			LocationIndicator3X3ClearComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), LocationIndicator3X3Clear, Location->GetActorLocation(), FRotator::ZeroRotator, FVector(1), false, false, ENCPoolMethod::AutoRelease);
		}

		if (Location->ActorHasTag(TEXT("LocationIndicator5X5")))
		{
			LocationIndicator5X5Component = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), LocationIndicator5X5, Location->GetActorLocation(), FRotator::ZeroRotator, FVector(1), false, false, ENCPoolMethod::ManualRelease);
		}

		if (Location->ActorHasTag(TEXT("TrainingDummy")))
		{
			const ASMTrainingDummy* TrainingDummyCDO = TrainingDummyClass ? TrainingDummyClass->GetDefaultObject<ASMTrainingDummy>() : nullptr;
			const UCapsuleComponent* TrainingDummyCDOCapsule = TrainingDummyCDO ? TrainingDummyCDO->GetCapsuleComponent() : nullptr;
			const float TrainingDummyCDOCapsuleHalfHeight = TrainingDummyCDOCapsule ? TrainingDummyCDOCapsule->GetScaledCapsuleHalfHeight() : 0.0f;
			TrainingDummyLocation = Location->GetActorLocation() + FVector(0.0, 0.0, TrainingDummyCDOCapsuleHalfHeight);
		}
	}
}

void USMTutorialManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	SetComponentTickEnabled(false);

	const UWorld* World = GetWorld();
	if (const APlayerController* PlayerController = World ? World->GetFirstPlayerController() : nullptr)
	{
		if (CachedTutorialUIControlComponent = PlayerController->GetComponentByClass<USMTutorialUIControlComponent>(); CachedTutorialUIControlComponent)
		{
			CachedTutorialUIControlComponent->SetGuideText(GetScriptText(CurrentStepNumber, 2, ESMCharacterType::None));
			CachedTutorialUIControlComponent->SetMissionText(GetScriptText(CurrentStepNumber, 1, ESMCharacterType::None));
			CachedTutorialUIControlComponent->PlayShowGuideAnimation();
			CachedTutorialUIControlComponent->ShowWASDKey();
		}

		// 캐릭터의 능력들을 일단 모두 잠그고 튜토리얼 진행에 따라 하나씩 풀어줍니다.
		const ASMGamePlayerState* PlayerState = PlayerController->GetPlayerState<ASMGamePlayerState>();
		if (UAbilitySystemComponent* ASC = PlayerState ? PlayerState->GetAbilitySystemComponent() : nullptr)
		{
			FGameplayTagContainer BlockingTags;
			BlockingTags.AddTag(SMTags::Character::State::Common::Blocking::Sampling);
			BlockingTags.AddTag(SMTags::Character::State::Common::Blocking::Attack);
			BlockingTags.AddTag(SMTags::Character::State::Common::Blocking::Skill);
			BlockingTags.AddTag(SMTags::Character::State::Common::Blocking::Hold);
			BlockingTags.AddTag(SMTags::Character::State::Common::Blocking::NoiseBreak);
			ASC->AddLooseGameplayTags(BlockingTags);
		}
	}

	if (MovePracticeTrigger.IsValid())
	{
		MovePracticeTrigger->OnActorBeginOverlap.AddDynamic(this, &ThisClass::OnStep1Completed);
	}

	if (EndTrigger.IsValid())
	{
		EndTrigger->OnActorBeginOverlap.AddDynamic(this, &ThisClass::OnStep11Completed);
	}

	for (TWeakObjectPtr<ASMTile> NoiseBreakTile : NoiseBreakTiles)
	{
		if (NoiseBreakTile.IsValid())
		{
			NoiseBreakTile->SetActorEnableCollision(false);
		}
	}

	for (TWeakObjectPtr<ASMTile> HealPackTile : HealPackTiles)
	{
		if (HealPackTile.IsValid())
		{
			HealPackTile->SetActorEnableCollision(false);
		}
	}

	for (TWeakObjectPtr<ASMTile> PreNextTile : PreNextSectionTiles)
	{
		if (PreNextTile.IsValid())
		{
			PreNextTile->SetActorEnableCollision(false);
		}
	}
}

void USMTutorialManagerComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (CurrentStepNumber == 9)
	{
		if (CachedCharacter = CachedCharacter ? CachedCharacter.Get() : Cast<ACharacter>(GetLocalPlayerPawn()); CachedCharacter)
		{
			CachedCharacter->AddMovementInput(FVector::RightVector, 1.0f, true);
		}

		if (CachedCharacter->GetActorLocation().Y >= -4200.0)
		{
			OnArrivedBattleZone();
		}
	}
}

void USMTutorialManagerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void USMTutorialManagerComponent::Activate(bool bReset)
{
	Super::Activate(bReset);

	TransformScriptsData();
}

APawn* USMTutorialManagerComponent::GetLocalPlayerPawn()
{
	const UWorld* World = GetWorld();
	const APlayerController* PlayerController = World ? World->GetFirstPlayerController() : nullptr;
	return PlayerController ? PlayerController->GetPawn() : nullptr;
}

void USMTutorialManagerComponent::TransformScriptsData()
{
	TMap<int32, TMap<int32, TMap<ESMCharacterType, FScriptData>>> UIScriptMap;
	for (const FName& RowName : TutorialScriptDataTable->GetRowNames())
	{
		if (const FSMTutorialScript* Row = TutorialScriptDataTable->FindRow<FSMTutorialScript>(RowName, TEXT("")))
		{
			FScriptData DialogueScript;
			DialogueScript.Ko = Row->Ko;
			DialogueScript.En = Row->En;
			DialogueScript.Ja = Row->Ja;

			if (Row->ScriptNumberInStep >= 0)
			{
				UIScriptMap.FindOrAdd(Row->Step).FindOrAdd(Row->ScriptNumberInStep).FindOrAdd(Row->PlayerCharacterType, DialogueScript);
			}
		}
	}

	// 파싱한 데이터를 사용하기 쉽게 가공합니다.
	auto Transform = [](const TMap<int32, TMap<int32, TMap<ESMCharacterType, FScriptData>>>& Map, TArray<TArray<TMap<ESMCharacterType, FScriptData>>>& OutScripts) {
		OutScripts.Reset();
		OutScripts.Push(TArray<TMap<ESMCharacterType, FScriptData>>()); // 0번 인덱스는 사용하지 않습니다.

		TArray<int> SortedSteps;
		Map.GetKeys(SortedSteps);
		SortedSteps.Sort();

		for (const int32 SortedStep : SortedSteps)
		{
			TArray<TMap<ESMCharacterType, FScriptData>> StepScripts;
			StepScripts.Push(TMap<ESMCharacterType, FScriptData>()); // 0번 인덱스는 사용하지 않습니다.

			TArray<int32> SortedScriptNumbers;
			Map[SortedStep].GetKeys(SortedScriptNumbers);
			SortedScriptNumbers.Sort();

			for (const int32 SortedScriptNumber : SortedScriptNumbers)
			{
				StepScripts.Add(Map[SortedStep][SortedScriptNumber]);
			}

			OutScripts.Add(StepScripts);
		}
	};

	Transform(UIScriptMap, UIScripts);

	for (int32 Step = 1; Step < UIScripts.Num(); ++Step)
	{
		UE_LOG(LogTemp, Verbose, TEXT("UI 스텝: %d"), Step);
		for (int32 ScriptsNumber = 1; ScriptsNumber < UIScripts[Step].Num(); ++ScriptsNumber)
		{
			for (const auto& [CharacterType, ScriptData] : UIScripts[Step][ScriptsNumber])
			{
				UE_LOG(LogTemp, Verbose, TEXT("%d: %s [%s]"), ScriptsNumber, *UIScripts[Step][ScriptsNumber][CharacterType].Ko, *UEnum::GetValueAsString(CharacterType));
			}
		}
	}
}

FString USMTutorialManagerComponent::GetScriptText(int32 StepNumber, int32 ScriptNumberInStep, ESMCharacterType CharacterType)
{
	if (UIScripts.IsValidIndex(StepNumber) && UIScripts[StepNumber].IsValidIndex(ScriptNumberInStep))
	{
		if (UIScripts[StepNumber][ScriptNumberInStep].Contains(CharacterType))
		{
			return UIScripts[StepNumber][ScriptNumberInStep][CharacterType].Ko;
		}

		return UIScripts[StepNumber][ScriptNumberInStep][ESMCharacterType::None].Ko;
	}

	return FString();
}

ESMCharacterType USMTutorialManagerComponent::GetLocalPlayerCharacterType()
{
	ASMPlayerCharacterBase* LocalCharacter = Cast<ASMPlayerCharacterBase>(GetLocalPlayerPawn());
	return LocalCharacter ? LocalCharacter->GetCharacterType() : ESMCharacterType::None;
}

void USMTutorialManagerComponent::OnStep1Completed(AActor* OverlappedActor, AActor* OtherActor)
{
	CurrentStepNumber = 2;

	if (OverlappedActor)
	{
		OverlappedActor->OnActorBeginOverlap.RemoveAll(this);
		OverlappedActor->Destroy();
	}

	if (CachedTutorialUIControlComponent)
	{
		CachedTutorialUIControlComponent->HideAllKeyInfo();

		const ESMCharacterType LocalCharacterType = GetLocalPlayerCharacterType();
		const FString GuideText = GetScriptText(CurrentStepNumber, 2, LocalCharacterType);
		const FString MissionText = GetScriptText(CurrentStepNumber, 1, LocalCharacterType);
		CachedTutorialUIControlComponent->TransitionAndSetText(GuideText, MissionText);
		CachedTutorialUIControlComponent->OnTransitionAndSetTextEnded.BindUObject(this, &ThisClass::OnStep2Started);
	}
}

void USMTutorialManagerComponent::OnStep2Started()
{
	TilesCaptureCount = 0;

	if (LocationIndicator3X3Component)
	{
		LocationIndicator3X3Component->Activate(true);
	}

	for (TWeakObjectPtr<ASMTile> SamplingTile : SamplingTiles)
	{
		if (SamplingTile.IsValid())
		{
			SamplingTile->OnChangeTile.AddDynamic(this, &ThisClass::OnSamplingTilesCaptured);
		}
	}

	if (UAbilitySystemComponent* ASC = USMAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetLocalPlayerPawn()))
	{
		ASC->RemoveLooseGameplayTag(SMTags::Character::State::Common::Blocking::Sampling);
	}
}

void USMTutorialManagerComponent::OnSamplingTilesCaptured()
{
	if (++TilesCaptureCount >= TargetTilesCaptureCountForStep2)
	{
		OnStep2Completed();
	}
}

void USMTutorialManagerComponent::OnStep2Completed()
{
	CurrentStepNumber = 3;

	if (LocationIndicator3X3ClearComponent)
	{
		LocationIndicator3X3ClearComponent->Activate(true);
	}

	if (LocationIndicator3X3Component)
	{
		LocationIndicator3X3Component->Deactivate();
		LocationIndicator3X3Component->ReleaseToPool();
		LocationIndicator3X3Component = nullptr;
	}

	for (TWeakObjectPtr<ASMTile> SamplingTile : SamplingTiles)
	{
		if (SamplingTile.IsValid())
		{
			SamplingTile->OnChangeTile.RemoveAll(this);
		}
	}

	if (CachedTutorialUIControlComponent)
	{
		const ESMCharacterType LocalCharacterType = GetLocalPlayerCharacterType();
		const FString GuideText = GetScriptText(CurrentStepNumber, 2, LocalCharacterType);
		const FString MissionText = GetScriptText(CurrentStepNumber, 1, LocalCharacterType);
		CachedTutorialUIControlComponent->TransitionAndSetText(GuideText, MissionText);
		CachedTutorialUIControlComponent->OnTransitionAndSetTextEnded.BindUObject(this, &ThisClass::OnStep3Started);
	}
}

void USMTutorialManagerComponent::OnStep3Started()
{
	if (UAbilitySystemComponent* ASC = USMAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetLocalPlayerPawn()))
	{
		ASC->RemoveLooseGameplayTag(SMTags::Character::State::Common::Blocking::Attack);
	}

	UWorld* World = GetWorld();
	TrainingDummy = World ? World->SpawnActor<ASMTrainingDummy>(TrainingDummyClass, TrainingDummyLocation, FVector::LeftVector.ToOrientationRotator()) : nullptr;

	if (CachedTutorialUIControlComponent)
	{
		CachedTutorialUIControlComponent->ShowLeftClick();
	}

	if (SamplingWall.IsValid())
	{
		SamplingWall->SetActorHiddenInGame(true);
		SamplingWall->Destroy();
	}

	if (TrainingDummy)
	{
		TrainingDummy->OnHalfHPReached.BindUObject(this, &ThisClass::OnStep3Completed);
	}

	if (HoldWall.IsValid())
	{
		HoldWall->SetActorHiddenInGame(false);
	}
}

void USMTutorialManagerComponent::OnStep3Completed()
{
	CurrentStepNumber = 4;

	if (TrainingDummy)
	{
		TrainingDummy->OnHalfHPReached.Unbind();
		TrainingDummy->SetInvincible(true);
		TrainingDummy->SetCurrentHP(50.0f);
	}

	if (CachedTutorialUIControlComponent)
	{
		CachedTutorialUIControlComponent->HideAllKeyInfo();

		const ESMCharacterType LocalCharacterType = GetLocalPlayerCharacterType();
		const FString GuideText = GetScriptText(CurrentStepNumber, 2, LocalCharacterType);
		const FString MissionText = GetScriptText(CurrentStepNumber, 1, LocalCharacterType);
		CachedTutorialUIControlComponent->TransitionAndSetText(GuideText, MissionText);
		CachedTutorialUIControlComponent->OnTransitionAndSetTextEnded.BindUObject(this, &ThisClass::OnStep4Started);
	}
}

void USMTutorialManagerComponent::OnStep4Started()
{
	USMAbilitySystemComponent* OwnerASC = USMAbilitySystemBlueprintLibrary::GetSMAbilitySystemComponent(GetLocalPlayerPawn());
	if (USMGA_Skill* SkillInstance = OwnerASC ? OwnerASC->GetGAInstanceFromClass<USMGA_Skill>() : nullptr)
	{
		OwnerASC->AddLooseGameplayTag(SMTags::Character::State::Common::UnlimitSkillGauge);

		SkillInstance->OnSkillHit.AddDynamic(this, &ThisClass::OnStep4Completed);
		OwnerASC->RemoveLooseGameplayTag(SMTags::Character::State::Common::Blocking::Skill);
	}

	if (CachedTutorialUIControlComponent)
	{
		CachedTutorialUIControlComponent->ShowEKey();
	}
}

void USMTutorialManagerComponent::OnStep4Completed()
{
	CurrentStepNumber = 5;

	USMAbilitySystemComponent* OwnerASC = USMAbilitySystemBlueprintLibrary::GetSMAbilitySystemComponent(GetLocalPlayerPawn());
	if (USMGA_Skill* SkillInstance = OwnerASC ? OwnerASC->GetGAInstanceFromClass<USMGA_Skill>() : nullptr)
	{
		OwnerASC->RemoveLooseGameplayTag(SMTags::Character::State::Common::UnlimitSkillGauge);
		SkillInstance->OnSkillHit.RemoveAll(this);
	}

	if (CachedTutorialUIControlComponent)
	{
		CachedTutorialUIControlComponent->HideAllKeyInfo();

		const ESMCharacterType LocalCharacterType = GetLocalPlayerCharacterType();
		const FString GuideText = GetScriptText(CurrentStepNumber, 2, LocalCharacterType);
		const FString MissionText = GetScriptText(CurrentStepNumber, 1, LocalCharacterType);
		CachedTutorialUIControlComponent->TransitionAndSetText(GuideText, MissionText);
		CachedTutorialUIControlComponent->OnTransitionAndSetTextEnded.BindUObject(this, &ThisClass::OnStep5Started);
	}
}

void USMTutorialManagerComponent::OnStep5Started()
{
	if (TrainingDummy)
	{
		TrainingDummy->SetInvincible(false);
		TrainingDummy->OnNeutralized.BindUObject(this, &ThisClass::OnStep5Completed);
	}
}

void USMTutorialManagerComponent::OnStep5Completed()
{
	CurrentStepNumber = 6;

	if (TrainingDummy)
	{
		TrainingDummy->OnNeutralized.Unbind();
	}

	if (CachedTutorialUIControlComponent)
	{
		const ESMCharacterType LocalCharacterType = GetLocalPlayerCharacterType();
		const FString GuideText = GetScriptText(CurrentStepNumber, 2, LocalCharacterType);
		const FString MissionText = GetScriptText(CurrentStepNumber, 1, LocalCharacterType);
		CachedTutorialUIControlComponent->TransitionAndSetText(GuideText, MissionText);
		CachedTutorialUIControlComponent->OnTransitionAndSetTextEnded.BindUObject(this, &ThisClass::OnStep6Started);
	}
}

void USMTutorialManagerComponent::OnStep6Started()
{
	USMAbilitySystemComponent* ASC = USMAbilitySystemBlueprintLibrary::GetSMAbilitySystemComponent(GetLocalPlayerPawn());
	if (USMGA_Hold* HoldInstance = ASC ? ASC->GetGAInstanceFromClass<USMGA_Hold>() : nullptr)
	{
		HoldInstance->OnHoldSucceed.AddDynamic(this, &ThisClass::OnStep6Completed);
		ASC->RemoveLooseGameplayTag(SMTags::Character::State::Common::Blocking::Hold);
	}

	if (CachedTutorialUIControlComponent)
	{
		CachedTutorialUIControlComponent->ShowRightClick();
	}
}

void USMTutorialManagerComponent::OnStep6Completed()
{
	CurrentStepNumber = 7;

	const USMAbilitySystemComponent* ASC = USMAbilitySystemBlueprintLibrary::GetSMAbilitySystemComponent(GetLocalPlayerPawn());
	if (USMGA_Hold* HoldInstance = ASC ? ASC->GetGAInstanceFromClass<USMGA_Hold>() : nullptr)
	{
		HoldInstance->OnHoldSucceed.RemoveAll(this);
	}

	if (LocationIndicator5X5Component)
	{
		LocationIndicator5X5Component->Activate(true);
	}

	if (CachedTutorialUIControlComponent)
	{
		CachedTutorialUIControlComponent->HideAllKeyInfo();

		const ESMCharacterType LocalCharacterType = GetLocalPlayerCharacterType();
		const FString GuideText = GetScriptText(CurrentStepNumber, 2, LocalCharacterType);
		const FString MissionText = GetScriptText(CurrentStepNumber, 1, LocalCharacterType);
		CachedTutorialUIControlComponent->TransitionAndSetText(GuideText, MissionText);
		CachedTutorialUIControlComponent->OnTransitionAndSetTextEnded.BindUObject(this, &ThisClass::OnStep7Started);
	}
}

void USMTutorialManagerComponent::OnStep7Started()
{
	USMAbilitySystemComponent* ASC = USMAbilitySystemBlueprintLibrary::GetSMAbilitySystemComponent(GetLocalPlayerPawn());
	if (USMGA_NoiseBreak* NoiseBreakInstance = ASC ? ASC->GetGAInstanceFromClass<USMGA_NoiseBreak>() : nullptr)
	{
		NoiseBreakInstance->OnNoiseBreakSucceed.AddDynamic(this, &ThisClass::OnTileCapturedByNoiseBreak);
		ASC->RemoveLooseGameplayTag(SMTags::Character::State::Common::Blocking::NoiseBreak);
	}

	if (CachedTutorialUIControlComponent)
	{
		CachedTutorialUIControlComponent->ShowLeftClick();
	}

	for (TWeakObjectPtr<ASMTile> NoiseBreakTile : NoiseBreakTiles)
	{
		if (NoiseBreakTile.IsValid())
		{
			NoiseBreakTile->SetActorEnableCollision(true);
		}
	}

	for (TWeakObjectPtr<ASMTile> NoiseBreakTile : NoiseBreakTiles)
	{
		if (NoiseBreakTile.IsValid())
		{
			NoiseBreakTile->SetActorEnableCollision(true);
		}
	}

	if (HoldWall.IsValid())
	{
		HoldWall->SetActorHiddenInGame(true);
		HoldWall->Destroy();
	}

	if (NoiseBreakWall.IsValid())
	{
		NoiseBreakWall->SetActorHiddenInGame(false);
	}
}

void USMTutorialManagerComponent::OnTileCapturedByNoiseBreak(TArray<ASMTile*> CapturedTiles)
{
	int32 ValidCapturedTilesCount = 0;
	for (TWeakObjectPtr<ASMTile> NoiseBreakTile : NoiseBreakTiles)
	{
		if (!NoiseBreakTile.IsValid())
		{
			continue;
		}

		if (CapturedTiles.Contains(NoiseBreakTile.Get()))
		{
			++ValidCapturedTilesCount;
		}
	}

	const int32 TargetCapturedCount = NoiseBreakTiles.Num() / 2;
	if (TargetCapturedCount <= ValidCapturedTilesCount)
	{
		OnStep7Completed();
	}
	else
	{
		RestartStep7();
	}
}

void USMTutorialManagerComponent::RestartStep7()
{
	for (TWeakObjectPtr<ASMTile> NoiseBreakTile : NoiseBreakTiles)
	{
		if (NoiseBreakTile.IsValid())
		{
			NoiseBreakTile->TileTrigger(ESMTeam::None);
		}
	}

	if (APawn* OwnerPawn = GetLocalPlayerPawn())
	{
		const UCapsuleComponent* OwnerCapsule = OwnerPawn->GetComponentByClass<UCapsuleComponent>();
		const float CapsuleZ = OwnerCapsule ? OwnerCapsule->GetScaledCapsuleHalfHeight() : 0.0f;
		const FVector TargetLocation = NoiseBreakRestartLocation + FVector(0.0, 0.0, CapsuleZ);
		OwnerPawn->SetActorLocation(TargetLocation);
	}

	if (TrainingDummy)
	{
		TrainingDummy->SetActorLocation(TrainingDummyLocation);
		TrainingDummy->ReceiveDamage(nullptr, 999.0f);
	}
}

void USMTutorialManagerComponent::OnStep7Completed()
{
	CurrentStepNumber = 8;

	if (LocationIndicator5X5Component)
	{
		LocationIndicator5X5Component->Deactivate();
		LocationIndicator5X5Component->ReleaseToPool();
		LocationIndicator5X5Component = nullptr;
	}

	if (TrainingDummy)
	{
		TrainingDummy->Destroy();
	}

	USMAbilitySystemComponent* ASC = USMAbilitySystemBlueprintLibrary::GetSMAbilitySystemComponent(GetLocalPlayerPawn());
	if (USMGA_NoiseBreak* NoiseBreakInstance = ASC ? ASC->GetGAInstanceFromClass<USMGA_NoiseBreak>() : nullptr)
	{
		ASC->AddLooseGameplayTag(SMTags::Character::State::Common::Blocking::NoiseBreak);
		NoiseBreakInstance->OnNoiseBreakSucceed.RemoveAll(this);
	}

	if (CachedTutorialUIControlComponent)
	{
		CachedTutorialUIControlComponent->HideAllKeyInfo();

		const ESMCharacterType LocalCharacterType = GetLocalPlayerCharacterType();
		const FString GuideText = GetScriptText(CurrentStepNumber, 2, LocalCharacterType);
		const FString MissionText = GetScriptText(CurrentStepNumber, 1, LocalCharacterType);
		CachedTutorialUIControlComponent->TransitionAndSetText(GuideText, MissionText);
		CachedTutorialUIControlComponent->OnTransitionAndSetTextEnded.BindUObject(this, &ThisClass::OnStep8Started);
	}
}

void USMTutorialManagerComponent::OnStep8Started()
{
	ASMPlayerCharacterBase* OwnerCharacter = Cast<ASMPlayerCharacterBase>(GetLocalPlayerPawn());
	USMAbilitySystemComponent* OwnerASC = USMAbilitySystemBlueprintLibrary::GetSMAbilitySystemComponent(OwnerCharacter);
	const USMPlayerCharacterDataAsset* DataAsset = OwnerCharacter->GetDataAsset();
	if (OwnerASC && DataAsset)
	{
		const FGameplayEffectSpecHandle GESpecHandle = OwnerASC->MakeOutgoingSpec(DataAsset->DamageGE, 1.0f, OwnerASC->MakeEffectContext());
		if (GESpecHandle.IsValid())
		{
			GESpecHandle.Data->SetSetByCallerMagnitude(SMTags::AttributeSet::Damage, 50.0f);
			OwnerASC->BP_ApplyGameplayEffectSpecToSelf(GESpecHandle);
		}
	}

	if (CachedTutorialUIControlComponent)
	{
		CachedTutorialUIControlComponent->ShowRightLeftClick();
	}

	if (OwnerASC)
	{
		const USMCharacterAttributeSet* OwnerAttributeSet = OwnerASC->GetSet<USMCharacterAttributeSet>();
		CachedOwnerMaxHP = OwnerAttributeSet ? OwnerAttributeSet->GetMaxPostureGauge() : 100.0f;
		OwnerASC->GetGameplayAttributeValueChangeDelegate(USMCharacterAttributeSet::GetPostureGaugeAttribute()).AddUObject(this, &ThisClass::OnHPChanged);
		OwnerASC->RemoveLooseGameplayTag(SMTags::Character::State::Common::Blocking::NoiseBreak);
	}

	if (NoiseBreakWall.IsValid())
	{
		NoiseBreakWall->SetActorHiddenInGame(true);
		NoiseBreakWall->Destroy();
	}

	if (HealPackWall.IsValid())
	{
		HealPackWall->SetActorHiddenInGame(false);
	}

	for (TWeakObjectPtr<ASMTile> HealPackTile : HealPackTiles)
	{
		if (HealPackTile.IsValid())
		{
			HealPackTile->SetActorEnableCollision(true);
		}
	}
}

void USMTutorialManagerComponent::OnHPChanged(const FOnAttributeChangeData& OnAttributeChangeData)
{
	if (OnAttributeChangeData.NewValue >= CachedOwnerMaxHP)
	{
		OnStep8Completed();
	}
}

void USMTutorialManagerComponent::OnStep8Completed()
{
	CurrentStepNumber = 9;

	USMAbilitySystemComponent* SourceASC = USMAbilitySystemBlueprintLibrary::GetSMAbilitySystemComponent(GetLocalPlayerPawn());
	if (USMGA_NoiseBreak* NoiseBreakInstance = SourceASC ? SourceASC->GetGAInstanceFromClass<USMGA_NoiseBreak>() : nullptr)
	{
		SourceASC->GetGameplayAttributeValueChangeDelegate(USMCharacterAttributeSet::GetPostureGaugeAttribute()).RemoveAll(this);
		NoiseBreakInstance->OnNoiseBreakSucceed.RemoveAll(this);
	}

	if (CachedTutorialUIControlComponent)
	{
		CachedTutorialUIControlComponent->HideAllKeyInfo();

		const ESMCharacterType LocalCharacterType = GetLocalPlayerCharacterType();
		const FString GuideText = GetScriptText(CurrentStepNumber, 2, LocalCharacterType);
		const FString MissionText = GetScriptText(CurrentStepNumber, 1, LocalCharacterType);
		CachedTutorialUIControlComponent->TransitionAndSetText(GuideText, MissionText);
		CachedTutorialUIControlComponent->OnTransitionAndSetTextEnded.BindUObject(this, &ThisClass::OnStep9Started);
	}
}

void USMTutorialManagerComponent::OnStep9Started()
{
	TilesCaptureCount = 0;

	if (HealPackWall.IsValid())
	{
		HealPackWall->Destroy();
	}

	if (NextTrigger.IsValid())
	{
		NextTrigger->OnActorBeginOverlap.AddDynamic(this, &ThisClass::OnNextTriggerBeginOverlap);
	}

	for (TWeakObjectPtr<ASMTile> PreNextTile : PreNextSectionTiles)
	{
		if (PreNextTile.IsValid())
		{
			PreNextTile->SetActorEnableCollision(true);
		}
	}
}

void USMTutorialManagerComponent::OnNextTriggerBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (OverlappedActor)
	{
		OverlappedActor->OnActorBeginOverlap.RemoveAll(this);
		OverlappedActor->Destroy();
	}

	if (USMAbilitySystemComponent* ASC = USMAbilitySystemBlueprintLibrary::GetSMAbilitySystemComponent(GetLocalPlayerPawn()))
	{
		ASC->AddLooseGameplayTag(SMTags::Character::State::Common::Uncontrollable);
	}

	SetComponentTickEnabled(true);
}

void USMTutorialManagerComponent::OnArrivedBattleZone()
{
	if (USMAbilitySystemComponent* ASC = USMAbilitySystemBlueprintLibrary::GetSMAbilitySystemComponent(GetLocalPlayerPawn()))
	{
		ASC->AddLooseGameplayTag(SMTags::Character::State::Common::Blocking::Sampling);
		ASC->RemoveLooseGameplayTag(SMTags::Character::State::Common::Uncontrollable);
	}

	if (NextSectionWall.IsValid())
	{
		NextSectionWall->SetActorHiddenInGame(false);
		NextSectionWall->SetActorEnableCollision(true);
	}

	SetComponentTickEnabled(false);

	OnStep9Completed();
}

void USMTutorialManagerComponent::OnStep9Completed()
{
	CurrentStepNumber = 10;

	if (CachedTutorialUIControlComponent)
	{
		const ESMCharacterType LocalCharacterType = GetLocalPlayerCharacterType();
		const FString GuideText = GetScriptText(CurrentStepNumber, 2, LocalCharacterType);
		const FString MissionText = GetScriptText(CurrentStepNumber, 1, LocalCharacterType);
		CachedTutorialUIControlComponent->TransitionAndSetText(GuideText, MissionText);
		CachedTutorialUIControlComponent->OnTransitionAndSetTextEnded.BindUObject(this, &ThisClass::OnStep10Started);
	}
}

void USMTutorialManagerComponent::OnStep10Started()
{
	if (BattleStartTrigger.IsValid())
	{
		BattleStartTrigger->OnActorBeginOverlap.AddDynamic(this, &ThisClass::StartBattle);
	}
}

void USMTutorialManagerComponent::StartBattle(AActor* OverlappedActor, AActor* OtherActor)
{
	if (USMAbilitySystemComponent* ASC = USMAbilitySystemBlueprintLibrary::GetSMAbilitySystemComponent(GetLocalPlayerPawn()))
	{
		ASC->RemoveLooseGameplayTag(SMTags::Character::State::Common::Blocking::Sampling);
	}

	if (OverlappedActor)
	{
		OverlappedActor->OnActorBeginOverlap.RemoveAll(this);
		OverlappedActor->Destroy();
	}

	BattleEntranceWall->SetActorHiddenInGame(false);
	BattleEntranceWall->SetActorEnableCollision(true);

	if (const UWorld* World = GetWorld())
	{
		FTimerHandle TimerHandle;
		World->GetTimerManager().SetTimer(TimerHandle, this, &ThisClass::OnStep10Completed, BattleTime);
	}

	if (UWorld* World = GetWorld())
	{
		AICharacter = World->SpawnActor<ASMAICharacterBase>(AICharacterClass, AISpawnLocation, FRotator::ZeroRotator);
		AICharacter->SpawnDefaultController();
	}
}

void USMTutorialManagerComponent::OnStep10Completed()
{
	CurrentStepNumber = 11;

	if (AICharacter)
	{
		AICharacter->Destroy();
	}
	if (CachedTutorialUIControlComponent)
	{
		const FString GuideText = TEXT("차량에 탑승");
		const FString MissionText = TEXT("우측 차량에 탑승해 튜토리얼을 종료하세요.");
		CachedTutorialUIControlComponent->TransitionAndSetText(GuideText, MissionText);
		CachedTutorialUIControlComponent->OnTransitionAndSetTextEnded.BindUObject(this, &ThisClass::OnStep11Started);
	}
}

void USMTutorialManagerComponent::OnStep11Started()
{
	if (BattleExitWall.IsValid())
	{
		BattleExitWall->Destroy();
	}
}

void USMTutorialManagerComponent::OnStep11Completed(AActor* OverlappedActor, AActor* OtherActor)
{
	if (OverlappedActor)
	{
		OverlappedActor->OnActorBeginOverlap.RemoveAll(this);
		OverlappedActor->Destroy();
	}

	APawn* OwnerPawn = GetLocalPlayerPawn();
	if (USMAbilitySystemComponent* OwnerASC = USMAbilitySystemBlueprintLibrary::GetSMAbilitySystemComponent(GetLocalPlayerPawn()))
	{
		OwnerASC->AddLooseGameplayTag(SMTags::Character::State::Common::Uncontrollable);
		OwnerPawn->SetActorHiddenInGame(true);
	}

	ALevelSequenceActor* LevelSequenceActor;
	if (VanSequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(GetWorld(), VanSequence, FMovieSceneSequencePlaybackSettings(), LevelSequenceActor); VanSequencePlayer)
	{
		VanSequencePlayer->Play();
		VanSequencePlayer->OnNativeFinished.BindLambda([ThisWeakPtr = MakeWeakObjectPtr(this)] {
			if (ThisWeakPtr.IsValid())
			{
				const UWorld* World = ThisWeakPtr->GetWorld();
				if (AGameModeBase* GameMode = World ? World->GetAuthGameMode() : nullptr)
				{
					GameMode->ReturnToMainMenuHost();
				}
			}
		});
	}
}
