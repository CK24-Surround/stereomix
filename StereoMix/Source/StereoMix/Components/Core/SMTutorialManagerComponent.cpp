// Copyright Studio Surround. All Rights Reserved.


#include "SMTutorialManagerComponent.h"

#include "EngineUtils.h"
#include "EnhancedInputComponent.h"
#include "SMTileManagerComponent.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "AbilitySystem/Abilities/Common/SMGA_Hold.h"
#include "AbilitySystem/Abilities/NoiseBreak/SMGA_NoiseBreak.h"
#include "Actors/Character/Player/SMPlayerCharacterBase.h"
#include "Actors/Tutorial/SMProgressTrigger.h"
#include "Actors/Tutorial/SMTrainingDummy.h"
#include "Components/PlayerController/SMTutorialUIControlComponent.h"
#include "Data/Character/SMPlayerCharacterDataAsset.h"
#include "Data/DataTable/Tutorial/SMTutorialScript.h"
#include "FunctionLibraries/SMAbilitySystemBlueprintLibrary.h"
#include "FunctionLibraries/SMTileFunctionLibrary.h"
#include "Utilities/SMLog.h"


USMTutorialManagerComponent::USMTutorialManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bAutoActivate = true;
}

void USMTutorialManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	const UWorld* World = GetWorld();
	if (APlayerController* PlayerController = World ? World->GetFirstPlayerController() : nullptr)
	{
		UEnhancedInputComponent* TutorialInputComponent = NewObject<UEnhancedInputComponent>(PlayerController);
		TutorialInputComponent->RegisterComponent();
		TutorialInputComponent->BindAction(NextInputAction, ETriggerEvent::Started, this, &ThisClass::OnNextInputReceived);
		PlayerController->PushInputComponent(TutorialInputComponent);

		if (APawn* PlayerPawn = PlayerController->GetPawn()) // 빙의되지 않은 경우 UI가 작동하지 않습니다. 따라서 빙의할때까지 기다립니다. 만약 이미 빙의되었다면 바로 호출합니다.
		{
			OnPossessedPawnChanged(nullptr, PlayerPawn);
		}
		else
		{
			PlayerController->OnPossessedPawnChanged.AddDynamic(this, &ThisClass::USMTutorialManagerComponent::OnPossessedPawnChanged);
		}
	}

	NET_VLOG(GetOwner(), 2, 30.0f, TEXT("목표: 목표지점으로 이동"));
}

void USMTutorialManagerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void USMTutorialManagerComponent::Activate(bool bReset)
{
	Super::Activate(bReset);

	TransformScriptsData();

	for (ASMProgressTrigger* ProgressTrigger : TActorRange<ASMProgressTrigger>(GetWorld()))
	{
		if (ProgressTrigger->ActorHasTag(TEXT("MovePractice")))
		{
			ProgressTrigger->OnActorBeginOverlap.AddDynamic(this, &ThisClass::OnStep1Completed);
		}
		else if (ProgressTrigger->ActorHasTag(TEXT("End")))
		{
			ProgressTrigger->OnActorBeginOverlap.AddDynamic(this, &ThisClass::OnStep10Completed);
		}
	}
}

void USMTutorialManagerComponent::ProcessTutorialDialogue()
{
	const UWorld* World = GetWorld();
	const APlayerController* PlayerController = World ? World->GetFirstPlayerController() : nullptr;
	if (!CachedTutorialUIControlComponent || !PlayerController)
	{
		return;
	}

	// 다이얼로그가 꺼져있다면 다시 다이얼로그를 활성화합니다.
	if (!CachedTutorialUIControlComponent->IsDialogueActivated())
	{
		CachedTutorialUIControlComponent->ActivateDialogue();
	}

	++CurrentScriptNumber; // 다음 스크립트를 읽기 위해 1을 더해줍니다.

	// 다이얼로그에 출력되야할 스크립트로 바꿔줍니다.
	if (DialogueScripts.IsValidIndex(CurrentStepNumber) && DialogueScripts[CurrentStepNumber].IsValidIndex(CurrentScriptNumber))
	{
		if (DialogueScripts[CurrentStepNumber][CurrentScriptNumber].Contains(ESMCharacterType::None))
		{
			UE_LOG(LogTemp, Warning, TEXT("%s"), *DialogueScripts[CurrentStepNumber][CurrentScriptNumber][ESMCharacterType::None].Ko);
			CachedTutorialUIControlComponent->SetScript(DialogueScripts[CurrentStepNumber][CurrentScriptNumber][ESMCharacterType::None].Ko);
		}
		else
		{
			if (const ASMPlayerCharacterBase* SourceCharacter = PlayerController ? PlayerController->GetPawn<ASMPlayerCharacterBase>() : nullptr)
			{
				const ESMCharacterType CharacterType = SourceCharacter->GetCharacterType();
				CachedTutorialUIControlComponent->SetScript(DialogueScripts[CurrentStepNumber][CurrentScriptNumber][CharacterType].Ko);
			}
		}
	}
	else // 만약 스크립트 넘버가 마지막이라면 다이얼로그를 종료하고 스크립트 넘버를 1로 초기화해줍니다.
	{
		CachedTutorialUIControlComponent->DeactivateDialogue();
		CurrentScriptNumber = 0;
	}
}

void USMTutorialManagerComponent::OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	if (!NewPawn)
	{
		return;
	}

	const UWorld* World = GetWorld();
	const APlayerController* PlayerController = World ? World->GetFirstPlayerController() : nullptr;
	CachedTutorialUIControlComponent = PlayerController ? PlayerController->GetComponentByClass<USMTutorialUIControlComponent>() : nullptr;

	ProcessTutorialDialogue();
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
		UE_LOG(LogTemp, Warning, TEXT("다이얼로그 스텝: %d"), Step);
		for (int32 ScriptsNumber = 1; ScriptsNumber < DialogueScripts[Step].Num(); ++ScriptsNumber)
		{
			for (const auto& [CharacterType, ScriptData] : DialogueScripts[Step][ScriptsNumber])
			{
				UE_LOG(LogTemp, Warning, TEXT("%d: %s [%s]"), ScriptsNumber, *DialogueScripts[Step][ScriptsNumber][CharacterType].Ko, *UEnum::GetValueAsString(CharacterType));
			}
		}
	}

	for (int32 Step = 1; Step < UIScripts.Num(); ++Step)
	{
		UE_LOG(LogTemp, Warning, TEXT("UI 스텝: %d"), Step);
		for (int32 ScriptsNumber = 1; ScriptsNumber < UIScripts[Step].Num(); ++ScriptsNumber)
		{
			for (const auto& [CharacterType, ScriptData] : UIScripts[Step][ScriptsNumber])
			{
				UE_LOG(LogTemp, Warning, TEXT("%d: %s [%s]"), ScriptsNumber, *UIScripts[Step][ScriptsNumber][CharacterType].Ko, *UEnum::GetValueAsString(CharacterType));
			}
		}
	}
}

void USMTutorialManagerComponent::OnNextInputReceived()
{
	ProcessTutorialDialogue();
}

void USMTutorialManagerComponent::OnStep1Completed(AActor* OverlappedActor, AActor* OtherActor)
{
	if (OverlappedActor)
	{
		OverlappedActor->OnActorBeginOverlap.RemoveAll(this);
		OverlappedActor->Destroy();
	}

	CurrentStepNumber = 2;
	ProcessTutorialDialogue();

	if (USMTileManagerComponent* TileManager = USMTileFunctionLibrary::GetTileManagerComponent(GetWorld()))
	{
		TileManager->OnTilesCaptured.AddDynamic(this, &ThisClass::OnTilesCaptured);
		TilesCaptureCount = 0;
	}
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
	CurrentStepNumber = 3;
	ProcessTutorialDialogue();

	for (ASMTrainingDummy* TrainingDummy : TActorRange<ASMTrainingDummy>(GetWorld()))
	{
		TrainingDummy->OnHalfHPReached.BindUObject(this, &ThisClass::OnStep3Completed);
	}
}

void USMTutorialManagerComponent::OnStep3Completed()
{
	for (ASMTrainingDummy* TrainingDummy : TActorRange<ASMTrainingDummy>(GetWorld()))
	{
		TrainingDummy->OnHalfHPReached.Unbind();
		TrainingDummy->SetInvincible(true);
	}

	CurrentStepNumber = 4;
	ProcessTutorialDialogue();

	const UWorld* World = GetWorld();
	const APlayerController* PlayerController = World ? World->GetFirstPlayerController() : nullptr;
	if (ASMPlayerCharacterBase* SourceCharacter = PlayerController ? PlayerController->GetPawn<ASMPlayerCharacterBase>() : nullptr)
	{
		SourceCharacter->OnSkillHitSucceed.AddUObject(this, &ThisClass::OnStep4Completed);
	}
}

void USMTutorialManagerComponent::OnStep4Completed()
{
	const UWorld* World = GetWorld();
	const APlayerController* PlayerController = World ? World->GetFirstPlayerController() : nullptr;
	if (ASMPlayerCharacterBase* SourceCharacter = PlayerController ? PlayerController->GetPawn<ASMPlayerCharacterBase>() : nullptr)
	{
		SourceCharacter->OnSkillHitSucceed.RemoveAll(this);
	}

	for (ASMTrainingDummy* TrainingDummy : TActorRange<ASMTrainingDummy>(GetWorld()))
	{
		TrainingDummy->SetInvincible(false);
	}

	CurrentStepNumber = 5;
	ProcessTutorialDialogue();

	for (ASMTrainingDummy* TrainingDummy : TActorRange<ASMTrainingDummy>(GetWorld()))
	{
		TrainingDummy->OnNeutralized.BindUObject(this, &ThisClass::OnStep5Completed);
	}
}

void USMTutorialManagerComponent::OnStep5Completed()
{
	for (ASMTrainingDummy* TrainingDummy : TActorRange<ASMTrainingDummy>(GetWorld()))
	{
		TrainingDummy->OnNeutralized.Unbind();
	}

	CurrentStepNumber = 6;
	ProcessTutorialDialogue();

	const UWorld* World = GetWorld();
	const APlayerController* PlayerController = World ? World->GetFirstPlayerController() : nullptr;
	APawn* Pawn = PlayerController ? PlayerController->GetPawn() : nullptr;
	const USMAbilitySystemComponent* SourceASC = USMAbilitySystemBlueprintLibrary::GetSMAbilitySystemComponent(Pawn);
	if (USMGA_Hold* HoldInstance = SourceASC ? SourceASC->GetGAInstanceFromClass<USMGA_Hold>() : nullptr)
	{
		HoldInstance->OnHoldCast.AddDynamic(this, &ThisClass::OnStep6Completed);
	}
}

void USMTutorialManagerComponent::OnStep6Completed()
{
	const UWorld* World = GetWorld();
	const APlayerController* PlayerController = World ? World->GetFirstPlayerController() : nullptr;
	APawn* Pawn = PlayerController ? PlayerController->GetPawn() : nullptr;
	const USMAbilitySystemComponent* SourceASC = USMAbilitySystemBlueprintLibrary::GetSMAbilitySystemComponent(Pawn);
	if (USMGA_Hold* HoldInstance = SourceASC ? SourceASC->GetGAInstanceFromClass<USMGA_Hold>() : nullptr)
	{
		HoldInstance->OnHoldCast.RemoveAll(this);
	}

	CurrentStepNumber = 7;
	ProcessTutorialDialogue();

	if (USMGA_NoiseBreak* NoiseBreakInstance = SourceASC ? SourceASC->GetGAInstanceFromClass<USMGA_NoiseBreak>() : nullptr)
	{
		NoiseBreakInstance->OnNoiseBreakCast.AddDynamic(this, &ThisClass::OnStep7Completed);
	}
}

void USMTutorialManagerComponent::OnStep7Completed()
{
	const UWorld* World = GetWorld();
	const APlayerController* PlayerController = World ? World->GetFirstPlayerController() : nullptr;
	ASMPlayerCharacterBase* Character = PlayerController ? PlayerController->GetPawn<ASMPlayerCharacterBase>() : nullptr;
	USMAbilitySystemComponent* SourceASC = USMAbilitySystemBlueprintLibrary::GetSMAbilitySystemComponent(Character);
	if (USMGA_NoiseBreak* NoiseBreakInstance = SourceASC ? SourceASC->GetGAInstanceFromClass<USMGA_NoiseBreak>() : nullptr)
	{
		NoiseBreakInstance->OnNoiseBreakCast.RemoveAll(this);
	}

	CurrentStepNumber = 8;
	ProcessTutorialDialogue();

	const USMPlayerCharacterDataAsset* DataAsset = Character->GetDataAsset();
	if (SourceASC && DataAsset)
	{
		const FGameplayEffectSpecHandle GESpecHandle = SourceASC->MakeOutgoingSpec(DataAsset->DamageGE, 1.0f, SourceASC->MakeEffectContext());
		if (GESpecHandle.IsValid())
		{
			GESpecHandle.Data->SetSetByCallerMagnitude(SMTags::AttributeSet::Damage, 50.0f);
			SourceASC->BP_ApplyGameplayEffectSpecToSelf(GESpecHandle);
		}
	}

	if (USMGA_NoiseBreak* NoiseBreakInstance = SourceASC ? SourceASC->GetGAInstanceFromClass<USMGA_NoiseBreak>() : nullptr)
	{
		NoiseBreakInstance->OnNoiseBreakCast.AddDynamic(this, &ThisClass::OnStep8Completed);
	}
}

void USMTutorialManagerComponent::OnStep8Completed()
{
	const UWorld* World = GetWorld();
	const APlayerController* PlayerController = World ? World->GetFirstPlayerController() : nullptr;
	APawn* Pawn = PlayerController ? PlayerController->GetPawn() : nullptr;
	const USMAbilitySystemComponent* SourceASC = USMAbilitySystemBlueprintLibrary::GetSMAbilitySystemComponent(Pawn);
	if (USMGA_NoiseBreak* NoiseBreakInstance = SourceASC ? SourceASC->GetGAInstanceFromClass<USMGA_NoiseBreak>() : nullptr)
	{
		NoiseBreakInstance->OnNoiseBreakCast.RemoveAll(this);
	}

	for (ASMTrainingDummy* TrainingDummy : TActorRange<ASMTrainingDummy>(GetWorld()))
	{
		const FVector DummyLocation = TrainingDummy->GetActorLocation();
		TrainingDummy->SetActorLocation(FVector(-1800.0, -2780.0, DummyLocation.Z));
	}

	CurrentStepNumber = 9;
	ProcessTutorialDialogue();

	TilesCaptureCount = 0;
}

void USMTutorialManagerComponent::OnStep9Completed()
{
	CurrentStepNumber = 10;
	ProcessTutorialDialogue();
}

void USMTutorialManagerComponent::OnStep10Completed(AActor* OverlappedActor, AActor* OtherActor)
{
	if (CurrentStepNumber == 10)
	{
		OverlappedActor->OnActorBeginOverlap.RemoveAll(this);
		OverlappedActor->Destroy();
	}
}
