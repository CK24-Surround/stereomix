// Copyright Studio Surround. All Rights Reserved.


#include "SMTutorialManagerComponent.h"

#include "EngineUtils.h"
#include "EnhancedInputComponent.h"
#include "Actors/Tutorial/SMProgressTrigger.h"
#include "Components/PlayerController/SMTutorialUIControlComponent.h"
#include "Utilities/SMLog.h"


USMTutorialManagerComponent::USMTutorialManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bAutoActivate = true;

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
			UITutorialControlComponent->SetScript(Scripts[0]);
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
}

void USMTutorialManagerComponent::OnProgressTriggerBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (OverlappedActor)
	{
		OverlappedActor->OnActorBeginOverlap.RemoveDynamic(this, &USMTutorialManagerComponent::OnProgressTriggerBeginOverlap);
		OverlappedActor->Destroy();
	}

	UE_LOG(LogTemp, Warning, TEXT("목표완료 시점 PlayerController: %p"), GetWorld()->GetFirstPlayerController());
	NET_VLOG(GetOwner(), 3, 30.0f, TEXT("목표완료: 목표지점 도착"));
}

void USMTutorialManagerComponent::OnNextInputReceived()
{
	UE_LOG(LogTemp, Warning, TEXT("Next 입력"));
}
