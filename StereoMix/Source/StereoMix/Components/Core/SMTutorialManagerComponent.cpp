// Copyright Studio Surround. All Rights Reserved.


#include "SMTutorialManagerComponent.h"

#include "GameFramework/GameModeBase.h"
#include "EngineUtils.h"
#include "EnhancedInputComponent.h"
#include "SMTileManagerComponent.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "AbilitySystem/Abilities/Common/SMGA_Hold.h"
#include "AbilitySystem/Abilities/NoiseBreak/SMGA_NoiseBreak.h"
#include "AbilitySystem/Abilities/Skill/SMGA_Skill.h"
#include "Actors/Character/Player/SMPlayerCharacterBase.h"
#include "Actors/Tutorial/SMProgressTriggerBase.h"
#include "Actors/Tutorial/SMTrainingDummy.h"
#include "Actors/Tutorial/SMTutorialInvisibleWallBase.h"
#include "Components/PlayerController/SMTutorialUIControlComponent.h"
#include "Data/Character/SMPlayerCharacterDataAsset.h"
#include "Data/DataTable/Tutorial/SMTutorialScript.h"
#include "FunctionLibraries/SMAbilitySystemBlueprintLibrary.h"
#include "FunctionLibraries/SMTileFunctionLibrary.h"
#include "Utilities/SMLog.h"


USMTutorialManagerComponent::USMTutorialManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
	bAutoActivate = true;
}

void USMTutorialManagerComponent::InitializeComponent()
{
	Super::InitializeComponent();

	for (ASMTutorialInvisibleWallBase* TutorialInvisibleWall : TActorRange<ASMTutorialInvisibleWallBase>(GetWorld()))
	{
		if (!TutorialInvisibleWall)
		{
			continue;
		}

		if (TutorialInvisibleWall->ActorHasTag(TEXT("Sampling")))
		{
			UE_LOG(LogTemp, Warning, TEXT("%s"), *GetNameSafe(TutorialInvisibleWall));
			SamplingEventWall = TutorialInvisibleWall;
		}

		if (TutorialInvisibleWall->ActorHasTag(TEXT("Neutralize")))
		{
			UE_LOG(LogTemp, Warning, TEXT("%s"), *GetNameSafe(TutorialInvisibleWall));
			NeutralizeEventWall = TutorialInvisibleWall;
		}

		if (TutorialInvisibleWall->ActorHasTag(TEXT("NoiseBreak")))
		{
			UE_LOG(LogTemp, Warning, TEXT("%s"), *GetNameSafe(TutorialInvisibleWall));
			NoiseBreakEventWall = TutorialInvisibleWall;
		}

		if (TutorialInvisibleWall->ActorHasTag(TEXT("BattleStart")))
		{
			UE_LOG(LogTemp, Warning, TEXT("%s"), *GetNameSafe(TutorialInvisibleWall));
			BattleStartEventWall = TutorialInvisibleWall;
		}

		if (TutorialInvisibleWall->ActorHasTag(TEXT("BattleEnd")))
		{
			UE_LOG(LogTemp, Warning, TEXT("%s"), *GetNameSafe(TutorialInvisibleWall));
			BattleEndEventWall = TutorialInvisibleWall;
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
			UE_LOG(LogTemp, Warning, TEXT("%s"), *GetNameSafe(ProgressTrigger));
			MovePracticeTrigger = ProgressTrigger;
		}

		if (ProgressTrigger->ActorHasTag(TEXT("Next")))
		{
			UE_LOG(LogTemp, Warning, TEXT("%s"), *GetNameSafe(ProgressTrigger));
			NextTrigger = ProgressTrigger;
		}

		if (ProgressTrigger->ActorHasTag(TEXT("End")))
		{
			UE_LOG(LogTemp, Warning, TEXT("%s"), *GetNameSafe(ProgressTrigger));
			EndTrigger = ProgressTrigger;
		}
	}

	for (ASMTrainingDummy* TrainingDummyActor : TActorRange<ASMTrainingDummy>(GetWorld()))
	{
		UE_LOG(LogTemp, Warning, TEXT("%s"), *GetNameSafe(TrainingDummyActor));
		TrainingDummy = TrainingDummyActor;
	}
}

void USMTutorialManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	const UWorld* World = GetWorld();
	if (const APlayerController* PlayerController = World ? World->GetFirstPlayerController() : nullptr)
	{
		CachedTutorialUIControlComponent = PlayerController->GetComponentByClass<USMTutorialUIControlComponent>();
		CachedTutorialUIControlComponent->SetGuideText(TEXT("목표 지점으로 이동하기"));
		CachedTutorialUIControlComponent->SetMissionText(TEXT("WASD로 캐릭터를 움직일 수 있습니다. 목표지점으로 이동해보세요."));
	}

	if (MovePracticeTrigger.IsValid())
	{
		MovePracticeTrigger->OnActorBeginOverlap.AddDynamic(this, &ThisClass::OnStep1Completed);
	}

	if (EndTrigger.IsValid())
	{
		EndTrigger->OnActorBeginOverlap.AddDynamic(this, &ThisClass::OnStep10Completed);
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
	TMap<int32, TMap<int32, TMap<ESMCharacterType, FScriptData>>> DialogueScriptMap;
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
				DialogueScriptMap.FindOrAdd(Row->Step).FindOrAdd(Row->ScriptNumberInStep).FindOrAdd(Row->PlayerCharacterType, DialogueScript);
			}
			else
			{
				UIScriptMap.FindOrAdd(Row->Step).FindOrAdd(FMath::Abs(Row->ScriptNumberInStep)).FindOrAdd(Row->PlayerCharacterType, DialogueScript);
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

	Transform(DialogueScriptMap, DialogueScripts);
	Transform(UIScriptMap, UIScripts);

	for (int32 Step = 1; Step < DialogueScripts.Num(); ++Step)
	{
		UE_LOG(LogTemp, Verbose, TEXT("다이얼로그 스텝: %d"), Step);
		for (int32 ScriptsNumber = 1; ScriptsNumber < DialogueScripts[Step].Num(); ++ScriptsNumber)
		{
			for (const auto& [CharacterType, ScriptData] : DialogueScripts[Step][ScriptsNumber])
			{
				UE_LOG(LogTemp, Verbose, TEXT("%d: %s [%s]"), ScriptsNumber, *DialogueScripts[Step][ScriptsNumber][CharacterType].Ko, *UEnum::GetValueAsString(CharacterType));
			}
		}
	}

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

void USMTutorialManagerComponent::OnStep1Completed(AActor* OverlappedActor, AActor* OtherActor)
{
	if (OverlappedActor)
	{
		OverlappedActor->OnActorBeginOverlap.RemoveAll(this);
		OverlappedActor->Destroy();
	}

	if (CachedTutorialUIControlComponent)
	{
		CachedTutorialUIControlComponent->TransitionToSuccess();
	}

	if (const UWorld* World = GetWorld())
	{
		FTimerHandle TimerHandle;
		World->GetTimerManager().SetTimer(TimerHandle, this, &ThisClass::OnStep2Started, CompleteShowingTime);
	}

	if (USMTileManagerComponent* TileManager = USMTileFunctionLibrary::GetTileManagerComponent(GetWorld()))
	{
		TileManager->OnTilesCaptured.AddDynamic(this, &ThisClass::OnTilesCaptured);
	}
}

void USMTutorialManagerComponent::OnStep2Started()
{
	CurrentStepNumber = 2;

	if (CachedTutorialUIControlComponent)
	{
		CachedTutorialUIControlComponent->TransitionToGuide();
		CachedTutorialUIControlComponent->SetGuideText(TEXT("목표영역에 타일 9개 점령하기"));
		CachedTutorialUIControlComponent->SetMissionText(TEXT("이동 시 타일을 점령할 수 있습니다. 목표구역의 타일을 모두 점령해보세요."));
	}

	TilesCaptureCount = 0;
}

void USMTutorialManagerComponent::OnTilesCaptured(const AActor* CapturedInstigator, int32 CapturedTileCount)
{
	TilesCaptureCount += CapturedTileCount;

	if (CurrentStepNumber == 2)
	{
		if (TilesCaptureCount >= TargetTilesCaptureCountForStep2)
		{
			OnStep2Completed();
		}
	}

	if (CurrentStepNumber == 9)
	{
		if (TilesCaptureCount >= TargetTilesCaptureCountForStep9)
		{
			OnStep9Completed();
		}
	}
}

void USMTutorialManagerComponent::OnStep2Completed()
{
	if (CachedTutorialUIControlComponent)
	{
		CachedTutorialUIControlComponent->TransitionToSuccess();
	}

	if (const UWorld* World = GetWorld())
	{
		FTimerHandle TimerHandle;
		World->GetTimerManager().SetTimer(TimerHandle, this, &ThisClass::OnStep3Started, CompleteShowingTime);
	}

	if (TrainingDummy.IsValid())
	{
		TrainingDummy->OnHalfHPReached.BindUObject(this, &ThisClass::OnStep3Completed);
	}
}

void USMTutorialManagerComponent::OnStep3Started()
{
	CurrentStepNumber = 3;

	if (CachedTutorialUIControlComponent)
	{
		CachedTutorialUIControlComponent->TransitionToGuide();
		CachedTutorialUIControlComponent->SetGuideText(TEXT("적 캐릭터 체력 50 깎기"));
		CachedTutorialUIControlComponent->SetMissionText(TEXT("일렉기타의 일반 공격은 전방에 총알을 난사합니다. 좌클릭을 눌러 앞의 적을 공격해보세요."));
	}

	if (SamplingEventWall.IsValid())
	{
		SamplingEventWall->Destroy();
	}
}

void USMTutorialManagerComponent::OnStep3Completed()
{
	if (TrainingDummy.IsValid())
	{
		TrainingDummy->OnHalfHPReached.Unbind();
		TrainingDummy->SetInvincible(true);
	}

	if (CachedTutorialUIControlComponent)
	{
		CachedTutorialUIControlComponent->TransitionToSuccess();
	}

	if (const UWorld* World = GetWorld())
	{
		FTimerHandle TimerHandle;
		World->GetTimerManager().SetTimer(TimerHandle, this, &ThisClass::OnStep4Started, CompleteShowingTime);
	}

	const USMAbilitySystemComponent* SourceASC = USMAbilitySystemBlueprintLibrary::GetSMAbilitySystemComponent(GetLocalPlayerPawn());
	if (USMGA_Skill* SkillInstance = SourceASC ? SourceASC->GetGAInstanceFromClass<USMGA_Skill>() : nullptr)
	{
		SkillInstance->OnSkillHit.AddDynamic(this, &ThisClass::OnStep4Completed);
	}
}

void USMTutorialManagerComponent::OnStep4Started()
{
	CurrentStepNumber = 4;

	if (CachedTutorialUIControlComponent)
	{
		CachedTutorialUIControlComponent->TransitionToGuide();
		CachedTutorialUIControlComponent->SetGuideText(TEXT("적 캐릭터에게 일렉기타 스킬 맞추기"));
		CachedTutorialUIControlComponent->SetMissionText(TEXT("E 스킬을 누르면 스킬을 사용할 수 있습니다. 스킬을 다시 사용하려면 타일을 점령하여 스킬게이지를 채워야합니다. 적에게 \"마비탄\"을 적중시켜보세요"));
	}
}

void USMTutorialManagerComponent::OnStep4Completed()
{
	const USMAbilitySystemComponent* SourceASC = USMAbilitySystemBlueprintLibrary::GetSMAbilitySystemComponent(GetLocalPlayerPawn());
	if (USMGA_Skill* SkillInstance = SourceASC ? SourceASC->GetGAInstanceFromClass<USMGA_Skill>() : nullptr)
	{
		SkillInstance->OnSkillHit.RemoveAll(this);
	}

	if (TrainingDummy.IsValid())
	{
		TrainingDummy->SetInvincible(false);
	}

	if (CachedTutorialUIControlComponent)
	{
		CachedTutorialUIControlComponent->TransitionToSuccess();
	}

	if (const UWorld* World = GetWorld())
	{
		FTimerHandle TimerHandle;
		World->GetTimerManager().SetTimer(TimerHandle, this, &ThisClass::OnStep5Started, CompleteShowingTime);
	}

	if (TrainingDummy.IsValid())
	{
		TrainingDummy->OnNeutralized.BindUObject(this, &ThisClass::OnStep5Completed);
	}
}

void USMTutorialManagerComponent::OnStep5Started()
{
	CurrentStepNumber = 5;

	if (CachedTutorialUIControlComponent)
	{
		CachedTutorialUIControlComponent->TransitionToGuide();
		CachedTutorialUIControlComponent->SetGuideText(TEXT("적 캐릭터 무력화"));
		CachedTutorialUIControlComponent->SetMissionText(TEXT("적의 체력을 모두 깎으면 적을 무력화 시킬 수 있습니다. 공격과 스킬을 이용해 적을 무력화 시키세요."));
	}
}

void USMTutorialManagerComponent::OnStep5Completed()
{
	if (TrainingDummy.IsValid())
	{
		TrainingDummy->OnNeutralized.Unbind();
	}

	if (CachedTutorialUIControlComponent)
	{
		CachedTutorialUIControlComponent->TransitionToSuccess();
	}

	if (const UWorld* World = GetWorld())
	{
		FTimerHandle TimerHandle;
		World->GetTimerManager().SetTimer(TimerHandle, this, &ThisClass::OnStep6Started, CompleteShowingTime);
	}

	const USMAbilitySystemComponent* SourceASC = USMAbilitySystemBlueprintLibrary::GetSMAbilitySystemComponent(GetLocalPlayerPawn());
	if (USMGA_Hold* HoldInstance = SourceASC ? SourceASC->GetGAInstanceFromClass<USMGA_Hold>() : nullptr)
	{
		HoldInstance->OnHoldSucceed.AddDynamic(this, &ThisClass::OnStep6Completed);
	}

	if (NeutralizeEventWall.IsValid())
	{
		NeutralizeEventWall->Destroy();
	}
}

void USMTutorialManagerComponent::OnStep6Started()
{
	CurrentStepNumber = 6;

	if (CachedTutorialUIControlComponent)
	{
		CachedTutorialUIControlComponent->TransitionToGuide();
		CachedTutorialUIControlComponent->SetGuideText(TEXT("적 캐릭터 잡기"));
		CachedTutorialUIControlComponent->SetMissionText(TEXT("우클릭을 눌러 무력화 된 적을 잡을 수 있습니다. 앞의 적을 잡아보세요."));
	}
}

void USMTutorialManagerComponent::OnStep6Completed()
{
	const USMAbilitySystemComponent* SourceASC = USMAbilitySystemBlueprintLibrary::GetSMAbilitySystemComponent(GetLocalPlayerPawn());
	if (USMGA_Hold* HoldInstance = SourceASC ? SourceASC->GetGAInstanceFromClass<USMGA_Hold>() : nullptr)
	{
		HoldInstance->OnHoldSucceed.RemoveAll(this);
	}

	if (CachedTutorialUIControlComponent)
	{
		CachedTutorialUIControlComponent->TransitionToSuccess();
	}

	if (const UWorld* World = GetWorld())
	{
		FTimerHandle TimerHandle;
		World->GetTimerManager().SetTimer(TimerHandle, this, &ThisClass::OnStep7Started, CompleteShowingTime);
	}

	if (USMGA_NoiseBreak* NoiseBreakInstance = SourceASC ? SourceASC->GetGAInstanceFromClass<USMGA_NoiseBreak>() : nullptr)
	{
		NoiseBreakInstance->OnNoiseBreakSucceed.AddDynamic(this, &ThisClass::OnStep7Completed);
	}
}

void USMTutorialManagerComponent::OnStep7Started()
{
	CurrentStepNumber = 7;

	if (CachedTutorialUIControlComponent)
	{
		CachedTutorialUIControlComponent->TransitionToGuide();
		CachedTutorialUIControlComponent->SetGuideText(TEXT("-목표지점(5X5) 50%이상 점령\n-노이즈브레이크를 사용하여 점령"));
		CachedTutorialUIControlComponent->SetMissionText(TEXT("적을 잡은 상태에서, 좌클릭을 눌러 노이즈 브레이크를 발동할 수 있습니다. 노이즈 브레이크는 한번에 많은 타일을 점령할 수 있습니다. 노이즈 브레이크를 사용해 목표지점의 타일을 점령해보세요!"));
	}
}

void USMTutorialManagerComponent::OnStep7Completed()
{
	const USMAbilitySystemComponent* SourceASC = USMAbilitySystemBlueprintLibrary::GetSMAbilitySystemComponent(GetLocalPlayerPawn());
	if (USMGA_NoiseBreak* NoiseBreakInstance = SourceASC ? SourceASC->GetGAInstanceFromClass<USMGA_NoiseBreak>() : nullptr)
	{
		NoiseBreakInstance->OnNoiseBreakSucceed.RemoveAll(this);
	}

	if (CachedTutorialUIControlComponent)
	{
		CachedTutorialUIControlComponent->TransitionToSuccess();
	}

	if (const UWorld* World = GetWorld())
	{
		FTimerHandle TimerHandle;
		World->GetTimerManager().SetTimer(TimerHandle, this, &ThisClass::OnStep8Started, CompleteShowingTime);
	}

	if (USMGA_NoiseBreak* NoiseBreakInstance = SourceASC ? SourceASC->GetGAInstanceFromClass<USMGA_NoiseBreak>() : nullptr)
	{
		NoiseBreakInstance->OnNoiseBreakSucceed.AddDynamic(this, &ThisClass::OnStep8Completed);
	}

	if (NoiseBreakEventWall.IsValid())
	{
		NoiseBreakEventWall->Destroy();
	}
}

void USMTutorialManagerComponent::OnStep8Started()
{
	CurrentStepNumber = 8;

	if (CachedTutorialUIControlComponent)
	{
		CachedTutorialUIControlComponent->TransitionToGuide();
		CachedTutorialUIControlComponent->SetGuideText(TEXT("힐팩 아이템 사용 후 체력 모두회복"));
		CachedTutorialUIControlComponent->SetMissionText(TEXT("힐팩을 사용해 체력을 회복해보세요. 힐팩 또한 잡은 상태에서 좌클릭으로 사용할 수 있습니다. 시전범위에는 치유장판을 생성해 팀원을 치료할 수도 있습니다."));
	}

	ASMPlayerCharacterBase* SourceCharacter = Cast<ASMPlayerCharacterBase>(GetLocalPlayerPawn());
	USMAbilitySystemComponent* SourceASC = USMAbilitySystemBlueprintLibrary::GetSMAbilitySystemComponent(SourceCharacter);
	const USMPlayerCharacterDataAsset* DataAsset = SourceCharacter->GetDataAsset();
	if (SourceASC && DataAsset)
	{
		const FGameplayEffectSpecHandle GESpecHandle = SourceASC->MakeOutgoingSpec(DataAsset->DamageGE, 1.0f, SourceASC->MakeEffectContext());
		if (GESpecHandle.IsValid())
		{
			GESpecHandle.Data->SetSetByCallerMagnitude(SMTags::AttributeSet::Damage, 50.0f);
			SourceASC->BP_ApplyGameplayEffectSpecToSelf(GESpecHandle);
		}
	}
}

void USMTutorialManagerComponent::OnStep8Completed()
{
	const USMAbilitySystemComponent* SourceASC = USMAbilitySystemBlueprintLibrary::GetSMAbilitySystemComponent(GetLocalPlayerPawn());
	if (USMGA_NoiseBreak* NoiseBreakInstance = SourceASC ? SourceASC->GetGAInstanceFromClass<USMGA_NoiseBreak>() : nullptr)
	{
		NoiseBreakInstance->OnNoiseBreakSucceed.RemoveAll(this);
	}

	if (TrainingDummy.IsValid())
	{
		const FVector DummyLocation = TrainingDummy->GetActorLocation();
		TrainingDummy->SetActorLocation(FVector(-1800.0, -2780.0, DummyLocation.Z));
	}

	if (CachedTutorialUIControlComponent)
	{
		CachedTutorialUIControlComponent->TransitionToSuccess();
	}

	if (const UWorld* World = GetWorld())
	{
		FTimerHandle TimerHandle;
		World->GetTimerManager().SetTimer(TimerHandle, this, &ThisClass::OnStep9Started, CompleteShowingTime);
	}
}

void USMTutorialManagerComponent::OnStep9Started()
{
	CurrentStepNumber = 9;

	if (CachedTutorialUIControlComponent)
	{
		CachedTutorialUIControlComponent->TransitionToGuide();
		CachedTutorialUIControlComponent->SetGuideText(TEXT("적과 전투해서 100점 달성"));
		CachedTutorialUIControlComponent->SetMissionText(TEXT("훈련장에서 적과 전투하세요. 시간내에 적보다 많은 타일을 점령하면 승리합니다."));
	}
}

void USMTutorialManagerComponent::OnStep9Completed()
{
	OnStep10Started();
}

void USMTutorialManagerComponent::OnStep10Started()
{
	TilesCaptureCount = 0;

	CurrentStepNumber = 10;
}

void USMTutorialManagerComponent::OnStep10Completed(AActor* OverlappedActor, AActor* OtherActor)
{
	if (CurrentStepNumber == 10) // 만약 Step이 10이 아닌데 End존 진입시 무효입니다.
	{
		if (OverlappedActor)
		{
			OverlappedActor->OnActorBeginOverlap.RemoveAll(this);
			OverlappedActor->Destroy();
		}

		if (USMTileManagerComponent* TileManager = USMTileFunctionLibrary::GetTileManagerComponent(GetWorld()))
		{
			TileManager->OnTilesCaptured.RemoveAll(this);
		}

		const UWorld* World = GetWorld();
		if (AGameModeBase* GameMode = World ? World->GetAuthGameMode() : nullptr)
		{
			GameMode->ReturnToMainMenuHost();
		}
	}
}
