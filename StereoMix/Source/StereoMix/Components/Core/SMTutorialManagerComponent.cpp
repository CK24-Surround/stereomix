// Copyright Studio Surround. All Rights Reserved.


#include "SMTutorialManagerComponent.h"

#include "CommonTextBlock.h"
#include "EngineUtils.h"
#include "Actors/Tutorial/SMProgressTrigger.h"
#include "Components/PlayerController/SMTutorialUIControlComponent.h"
#include "UI/Widget/Tutorial/SMTutorialDialogue.h"
#include "UI/Widget/Tutorial/SMTutorialHUD.h"
#include "Utilities/SMLog.h"


USMTutorialManagerComponent::USMTutorialManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bAutoActivate = true;
}

void USMTutorialManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	const APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (USMTutorialUIControlComponent* UITutorialControlComponent = PlayerController ? PlayerController->GetComponentByClass<USMTutorialUIControlComponent>() : nullptr)
	{
		const FString Script = TEXT("따르릉! 따르릉! 뮤지션님! 지금 어디세요? 이러다가 공연에 늦겠어요! 뭣...작업하느라 아직 작업실이라고요? 지각하려고 작정하셨어요? 오른편에 벤 세워 뒀으니까, 빨리 뛰어오세요! WASD 로 이동할 수 있으니까 오른쪽으로 쭉 오세요! 서둘러요!");
		UITutorialControlComponent->SetScript(Script);
	}

	NET_VLOG(GetOwner(), 2, 30.0f, TEXT("목표: 목표지점으로 이동"));
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

	NET_VLOG(GetOwner(), 3, 30.0f, TEXT("목표완료: 목표지점 도착"));
}
