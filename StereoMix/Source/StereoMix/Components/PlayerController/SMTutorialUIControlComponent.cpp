// Copyright Studio Surround. All Rights Reserved.


#include "SMTutorialUIControlComponent.h"

#include "Controllers/SMGamePlayerController.h"
#include "UI/Widget/Tutorial/SMTutorialHUD.h"
#include "Utilities/SMLog.h"


USMTutorialUIControlComponent::USMTutorialUIControlComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bAutoActivate = true;
}

void USMTutorialUIControlComponent::Activate(bool bReset)
{
	Super::Activate(bReset);

	if (APlayerController* OwnerController = GetOwner<APlayerController>())
	{
		OwnerController->OnPossessedPawnChanged.AddDynamic(this, &ThisClass::OnPossessedPawnChanged);
	}
}

void USMTutorialUIControlComponent::OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	NET_LOG(GetOwner(), Warning, TEXT("폰 체인지"));

	if (!NewPawn)
	{
		return;
	}

	ASMGamePlayerController* PlayerController = GetOwner<ASMGamePlayerController>();
	if (TutorialHUD = PlayerController ? CreateWidget<USMTutorialHUD>(PlayerController, TutorialHUDClass) : nullptr; TutorialHUD) // 할당된 폰이 있는 경우에만 CreateWidget이 가능해 이 이벤트에서 처리해줍니다.
	{
		PlayerController->OnPossessedPawnChanged.RemoveAll(this);
		TutorialHUD->AddToViewport();
	}
}
