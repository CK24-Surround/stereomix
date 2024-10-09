// Copyright Studio Surround. All Rights Reserved.


#include "SMTutorialManagerComponent.h"

#include "EngineUtils.h"
#include "EnhancedInputComponent.h"
#include "Actors/Character/Player/SMPlayerCharacterBase.h"
#include "Actors/Tutorial/SMProgressTrigger.h"
#include "Components/PlayerController/SMTutorialUIControlComponent.h"
#include "Data/DataTable/Tutorial/SMTutorialScript.h"
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

	for (ASMProgressTrigger* ProgressTrigger : TActorRange<ASMProgressTrigger>(GetWorld()))
	{
		ProgressTrigger->OnActorBeginOverlap.AddDynamic(this, &ThisClass::OnProgressTriggerBeginOverlap);
	}

	TransformScriptsData();
}

void USMTutorialManagerComponent::OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	if (!NewPawn)
	{
		return;
	}

	PrintScript(CurrentStepNumber, CurrentScriptNumber++);
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
	UE_LOG(LogTemp, Warning, TEXT("Next 입력"));

	PrintScript(CurrentStepNumber, CurrentScriptNumber++);
}

void USMTutorialManagerComponent::OnProgressTriggerBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (OverlappedActor)
	{
		OverlappedActor->OnActorBeginOverlap.RemoveDynamic(this, &USMTutorialManagerComponent::OnProgressTriggerBeginOverlap);
		OverlappedActor->Destroy();
	}
}

void USMTutorialManagerComponent::PrintScript(int32 StepNumber, int32 ScriptsNumber)
{
	const UWorld* World = GetWorld();
	const APlayerController* PlayerController = World ? World->GetFirstPlayerController() : nullptr;
	USMTutorialUIControlComponent* UITutorialControlComponent = PlayerController ? PlayerController->GetComponentByClass<USMTutorialUIControlComponent>() : nullptr;
	if (!UITutorialControlComponent)
	{
		return;
	}

	if (!DialogueScripts.IsValidIndex(StepNumber) || !DialogueScripts[StepNumber].IsValidIndex(ScriptsNumber))
	{
		OnScriptsEnded();
		return;
	}

	if (DialogueScripts[StepNumber][ScriptsNumber].Contains(ESMCharacterType::None))
	{
		UE_LOG(LogTemp, Warning, TEXT("%s"), *DialogueScripts[StepNumber][ScriptsNumber][ESMCharacterType::None].Ko);
		UITutorialControlComponent->SetScript(DialogueScripts[StepNumber][ScriptsNumber][ESMCharacterType::None].Ko);
	}
	else
	{
		if (const ASMPlayerCharacterBase* SourceCharacter = PlayerController ? PlayerController->GetPawn<ASMPlayerCharacterBase>() : nullptr)
		{
			const ESMCharacterType CharacterType = SourceCharacter->GetCharacterType();
			UITutorialControlComponent->SetScript(DialogueScripts[StepNumber][ScriptsNumber][CharacterType].Ko);
		}
	}
}

void USMTutorialManagerComponent::OnScriptsEnded()
{
	const UWorld* World = GetWorld();
	const APlayerController* PlayerController = World ? World->GetFirstPlayerController() : nullptr;
	if (USMTutorialUIControlComponent* UITutorialControlComponent = PlayerController ? PlayerController->GetComponentByClass<USMTutorialUIControlComponent>() : nullptr)
	{
		UITutorialControlComponent->DeactivateDialogue();
	}

	++CurrentStepNumber;
}
