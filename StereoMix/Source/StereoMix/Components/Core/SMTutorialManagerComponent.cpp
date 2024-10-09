// Copyright Studio Surround. All Rights Reserved.


#include "SMTutorialManagerComponent.h"

#include "EngineUtils.h"
#include "EnhancedInputComponent.h"
#include "Actors/Tutorial/SMProgressTrigger.h"
#include "Components/PlayerController/SMTutorialUIControlComponent.h"
#include "Data/DataTable/Tutorial/SMTutorialScript.h"
#include "Utilities/SMLog.h"


USMTutorialManagerComponent::USMTutorialManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bAutoActivate = true;

	Scripts.Push(TEXT(""));
	Scripts.Push(TEXT("따르릉! 따르릉! 뮤지션님! 지금 어디세요? 이러다가 공연에 늦겠어요! 뭣...작업하느라 아직 작업실이라고요? 지각하려고 작정하셨어요? 오른편에 벤 세워 뒀으니까, 빨리 뛰어오세요! WASD 로 이동할 수 있으니까 오른쪽으로 쭉 오세요! 서둘러요!"));
	Scripts.Push(TEXT("뭣...작업하느라 아직 작업실이라고요? 지각하려고 작정하셨어요?"));
	Scripts.Push(TEXT("오른편에 벤 세워 뒀으니까, 빨리 뛰어오세요!"));
	Scripts.Push(TEXT("WASD 로 이동할 수 있으니까 오른쪽으로 쭉 오세요! 서둘러요!"));
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

		if (USMTutorialUIControlComponent* UITutorialControlComponent = PlayerController->GetComponentByClass<USMTutorialUIControlComponent>())
		{
			// if (DialogueScriptMap.Contains(TutorialStep))
			// {
			// 	DialogueScriptMap[TutorialStep].
			//
			//
			//
			// }
			UITutorialControlComponent->SetScript(Scripts[ScriptIndex++]);
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

void USMTutorialManagerComponent::TransformScriptsData()
{
	TMap<int32, TMap<int32, FScriptData>> DialogueScriptMap;
	TMap<int32, TMap<int32, FScriptData>> UIScriptMap;
	for (const FName& RowName : TutorialScriptDataTable->GetRowNames())
	{
		if (const FSMTutorialScript* Row = TutorialScriptDataTable->FindRow<FSMTutorialScript>(RowName, TEXT("")))
		{
			FScriptData DialogueScript;
			DialogueScript.PlayerCharacterType = Row->PlayerCharacterType;
			DialogueScript.Ko = Row->Ko;
			DialogueScript.En = Row->En;

			if (Row->ScriptNumberInStep >= 0)
			{
				DialogueScriptMap.FindOrAdd(Row->Step).FindOrAdd(Row->ScriptNumberInStep, DialogueScript);
			}
			else
			{
				UIScriptMap.FindOrAdd(Row->Step).FindOrAdd(FMath::Abs(Row->ScriptNumberInStep), DialogueScript);
			}
		}
	}

	auto ToArray = [](const TMap<int32, TMap<int32, FScriptData>>& Map, TArray<TArray<FScriptData>>& OutScripts) {
		OutScripts.Reset();
		OutScripts.Push(TArray<FScriptData>());

		TArray<int> SortedSteps;
		Map.GetKeys(SortedSteps);
		SortedSteps.Sort();

		for (const int32 SortedStep : SortedSteps)
		{
			TArray<FScriptData> StepScripts;
			StepScripts.Push(FScriptData());

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

	ToArray(DialogueScriptMap, DialogueScripts);
	ToArray(UIScriptMap, UIScripts);

	for (int32 Step = 0; Step < DialogueScripts.Num(); ++Step)
	{
		UE_LOG(LogTemp, Warning, TEXT("다이얼로그 스텝: %d"), Step);
		for (int32 ScriptsNumber = 0; ScriptsNumber < DialogueScripts[Step].Num(); ++ScriptsNumber)
		{
			UE_LOG(LogTemp, Warning, TEXT("%d: %s"), ScriptsNumber, *DialogueScripts[Step][ScriptsNumber].Ko);
		}
	}

	for (int32 Step = 0; Step < UIScripts.Num(); ++Step)
	{
		UE_LOG(LogTemp, Warning, TEXT("UI 스텝: %d"), Step);
		for (int32 ScriptsNumber = 0; ScriptsNumber < UIScripts[Step].Num(); ++ScriptsNumber)
		{
			UE_LOG(LogTemp, Warning, TEXT("%d: %s"), ScriptsNumber, *UIScripts[Step][ScriptsNumber].Ko);
		}
	}
}

void USMTutorialManagerComponent::OnNextInputReceived()
{
	UE_LOG(LogTemp, Warning, TEXT("Next 입력"));

	const UWorld* World = GetWorld();
	const APlayerController* PlayerController = World ? World->GetFirstPlayerController() : nullptr;
	if (USMTutorialUIControlComponent* UITutorialControlComponent = PlayerController ? PlayerController->GetComponentByClass<USMTutorialUIControlComponent>() : nullptr)
	{
		if (ScriptIndex > 0 && Scripts.IsValidIndex(ScriptIndex))
		{
			UITutorialControlComponent->SetScript(Scripts[ScriptIndex++]);
		}
		else
		{
			UITutorialControlComponent->DeactivateDialogue();
		}
	}
}

void USMTutorialManagerComponent::OnProgressTriggerBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (OverlappedActor)
	{
		OverlappedActor->OnActorBeginOverlap.RemoveDynamic(this, &USMTutorialManagerComponent::OnProgressTriggerBeginOverlap);
		OverlappedActor->Destroy();
	}
}
