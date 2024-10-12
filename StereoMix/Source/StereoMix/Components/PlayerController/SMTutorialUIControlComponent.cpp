// Copyright Studio Surround. All Rights Reserved.


#include "SMTutorialUIControlComponent.h"

#include "AbilitySystemComponent.h"
#include "CommonTextBlock.h"
#include "AbilitySystem/SMTags.h"
#include "Controllers/SMGamePlayerController.h"
#include "Games/SMGamePlayerState.h"
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

bool USMTutorialUIControlComponent::IsDialogueActivated()
{
	const USMTutorialDialogue* TutorialDialogue = TutorialHUD ? TutorialHUD->GetTutorialDialogue() : nullptr;
	return TutorialDialogue ? TutorialDialogue->IsActivated() : false;
}

void USMTutorialUIControlComponent::ActivateDialogue()
{
	const APlayerController* OwnerController = GetOwner<APlayerController>();
	const ASMGamePlayerState* PlayerState = OwnerController ? OwnerController->GetPlayerState<ASMGamePlayerState>() : nullptr;
	if (UAbilitySystemComponent* ASC = PlayerState ? PlayerState->GetAbilitySystemComponent() : nullptr)
	{
		ASC->AddLooseGameplayTag(SMTags::Character::State::Common::Uncontrollable);
	}

	if (USMTutorialDialogue* TutorialDialogue = TutorialHUD ? TutorialHUD->GetTutorialDialogue() : nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("다이얼로그 활성화"));
		TutorialDialogue->ActivateWidget();
		TutorialDialogue->SetVisibility(ESlateVisibility::Visible);
	}
}

void USMTutorialUIControlComponent::DeactivateDialogue()
{
	const APlayerController* OwnerController = GetOwner<APlayerController>();
	const ASMGamePlayerState* PlayerState = OwnerController ? OwnerController->GetPlayerState<ASMGamePlayerState>() : nullptr;
	if (UAbilitySystemComponent* ASC = PlayerState ? PlayerState->GetAbilitySystemComponent() : nullptr)
	{
		const int32 TagCountToRemove = ASC->GetTagCount(SMTags::Character::State::Common::Uncontrollable);
		if (TagCountToRemove > 0)
		{
			ASC->RemoveLooseGameplayTag(SMTags::Character::State::Common::Uncontrollable, TagCountToRemove);
		}
	}

	if (USMTutorialDialogue* TutorialDialogue = TutorialHUD ? TutorialHUD->GetTutorialDialogue() : nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("다이얼로그 비활성화"));
		TutorialDialogue->DeactivateWidget();
		TutorialDialogue->SetVisibility(ESlateVisibility::Hidden);
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
	if (TutorialHUD = PlayerController ? CreateWidget<USMTutorialHUD>(PlayerController, TutorialHUDClass) : nullptr; TutorialHUD) // 할당된 폰이 있는 경우에만 CreateWidget이 가능해 이 이벤트에서 처리해줍니다.
	{
		PlayerController->OnPossessedPawnChanged.RemoveAll(this);

		TutorialHUD->AddToViewport();
		ActivateDialogue();
	}
}
