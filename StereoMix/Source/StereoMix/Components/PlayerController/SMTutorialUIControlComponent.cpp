// Copyright Studio Surround. All Rights Reserved.


#include "SMTutorialUIControlComponent.h"

#include "CommonTextBlock.h"
#include "Controllers/SMGamePlayerController.h"
#include "UI/Widget/Tutorial/SMTutorialDialogue.h"
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

void USMTutorialUIControlComponent::SetScript(const FString& InString)
{
	const USMTutorialDialogue* TutorialDialogue = TutorialHUD ? TutorialHUD->GetTutorialDialogue() : nullptr;
	if (UCommonTextBlock* ScriptTextBlock = TutorialDialogue ? TutorialDialogue->ScriptTextBlock : nullptr)
	{
		ScriptTextBlock->SetText(FText::FromString(InString));
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
	if (TutorialHUD = PlayerController ? CreateWidget<USMTutorialHUD>(PlayerController, TutorialHUDClass) : nullptr; TutorialHUD)
	{
		TutorialHUD->AddToViewport();
	}
}
