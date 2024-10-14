// Copyright Studio Surround. All Rights Reserved.


#include "SMTutorialUIControlComponent.h"

#include "CommonTextBlock.h"
#include "UI/Widget/Tutorial/SMTutorialGuide.h"
#include "UI/Widget/Tutorial/SMTutorialHUD.h"
#include "UI/Widget/Tutorial/SMTutorialMission.h"
#include "UI/Widget/Tutorial/SMTutorialSuccess.h"


USMTutorialUIControlComponent::USMTutorialUIControlComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bAutoActivate = true;
}

void USMTutorialUIControlComponent::Activate(bool bReset)
{
	Super::Activate(bReset);

	if (TutorialHUD = CreateWidget<USMTutorialHUD>(GetWorld(), TutorialHUDClass); TutorialHUD) // 할당된 폰이 있는 경우에만 CreateWidget이 가능해 이 이벤트에서 처리해줍니다.
	{
		TutorialHUD->AddToViewport();

		if (TutorialHUD->TutorialSuccess)
		{
			TutorialHUD->TutorialSuccess->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

USMTutorialGuide* USMTutorialUIControlComponent::GetTutorialGuide() const
{
	return TutorialHUD ? TutorialHUD->TutorialGuide : nullptr;
}

USMTutorialSuccess* USMTutorialUIControlComponent::GetTutorialSuccess() const
{
	return TutorialHUD ? TutorialHUD->TutorialSuccess : nullptr;
}

USMTutorialMission* USMTutorialUIControlComponent::GetTutorialMission() const
{
	return TutorialHUD ? TutorialHUD->TutorialMission : nullptr;;
}

void USMTutorialUIControlComponent::SetGuideText(const FString& InString)
{
	USMTutorialGuide* TutorialGuide = GetTutorialGuide();
	if (UCommonTextBlock* GuideTextBlock = TutorialGuide ? TutorialGuide->GuideTextBlock : nullptr)
	{
		TutorialGuide->PlayAnimation(TutorialGuide->ShowGuideAnimation);
		GuideTextBlock->SetText(FText::FromString(InString));
	}
}

void USMTutorialUIControlComponent::SetMissionText(const FString& InString)
{
	USMTutorialMission* TutorialMission = GetTutorialMission();
	if (UCommonTextBlock* DetailTextBlock = TutorialMission ? TutorialMission->DetailTextBlock : nullptr)
	{
		TutorialMission->PlayAnimation(TutorialMission->ShowMissionAnimation);
		DetailTextBlock->SetText(FText::FromString(InString));
	}
}

void USMTutorialUIControlComponent::ShowGuide()
{
	if (USMTutorialGuide* TutorialGuide = GetTutorialGuide())
	{
		TutorialGuide->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		TutorialGuide->PlayAnimation(TutorialGuide->ShowGuideAnimation);
	}
}

void USMTutorialUIControlComponent::ShowMission()
{
	if (USMTutorialMission* TutorialMission = GetTutorialMission())
	{
		TutorialMission->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		TutorialMission->PlayAnimation(TutorialMission->ShowMissionAnimation);
	}
}

void USMTutorialUIControlComponent::ShowSuccess()
{
	if (USMTutorialSuccess* TutorialSuccess = GetTutorialSuccess())
	{
		TutorialSuccess->PlayAnimation(TutorialSuccess->ShowSuccessAnimation);
	}
}

void USMTutorialUIControlComponent::TransitionToSuccess()
{
	InternalTransitionToSuccess();
}

void USMTutorialUIControlComponent::TransitionToGuide()
{
	InternalTransitionToGuide();
}

void USMTutorialUIControlComponent::InternalTransitionToSuccess()
{
	UE_LOG(LogTemp, Warning, TEXT("목표 달성으로 전환 시작"));

	if (USMTutorialGuide* TutorialGuide = GetTutorialGuide())
	{
		TutorialGuide->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}

	if (USMTutorialSuccess* TutorialSuccess = GetTutorialSuccess())
	{
		TutorialSuccess->SetVisibility(ESlateVisibility::Hidden);
	}

	if (USMTutorialMission* TutorialMission = GetTutorialMission())
	{
		TutorialMission->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}

	if (USMTutorialMission* TutorialMission = GetTutorialMission())
	{
		if (TutorialMission->HideMissionAnimation)
		{
			FWidgetAnimationDynamicEvent EndDelegate;
			EndDelegate.BindDynamic(this, &ThisClass::OnHideMissionAnimationEnded);
			TutorialMission->UnbindAllFromAnimationFinished(TutorialMission->HideMissionAnimation);
			TutorialMission->BindToAnimationFinished(TutorialMission->HideMissionAnimation, EndDelegate);
			TutorialMission->PlayAnimation(TutorialMission->HideMissionAnimation);
		}
		else
		{
			OnHideMissionAnimationEnded();
		}
	}
}

void USMTutorialUIControlComponent::OnHideMissionAnimationEnded()
{
	UE_LOG(LogTemp, Warning, TEXT("미션 창 숨기기 애니메이션 종료"));

	if (USMTutorialMission* TutorialMission = GetTutorialMission())
	{
		TutorialMission->SetVisibility(ESlateVisibility::Hidden);
	}

	if (USMTutorialGuide* TutorialGuide = GetTutorialGuide())
	{
		if (TutorialGuide->HideGuideAnimation)
		{
			FWidgetAnimationDynamicEvent EndDelegate;
			EndDelegate.BindDynamic(this, &ThisClass::OnHideGuideAnimationEnded);
			TutorialGuide->UnbindAllFromAnimationFinished(TutorialGuide->HideGuideAnimation);
			TutorialGuide->BindToAnimationFinished(TutorialGuide->HideGuideAnimation, EndDelegate);
			TutorialGuide->PlayAnimation(TutorialGuide->HideGuideAnimation);
		}
		else
		{
			OnHideGuideAnimationEnded();
		}
	}
}

void USMTutorialUIControlComponent::OnHideGuideAnimationEnded()
{
	UE_LOG(LogTemp, Warning, TEXT("가이드 창 숨기기 애니메이션 종료"));

	if (USMTutorialGuide* TutorialGuide = GetTutorialGuide())
	{
		TutorialGuide->SetVisibility(ESlateVisibility::Hidden);
	}

	if (USMTutorialSuccess* TutorialSuccess = GetTutorialSuccess())
	{
		TutorialSuccess->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

		if (TutorialSuccess->ShowSuccessAnimation)
		{
			TutorialSuccess->PlayAnimation(TutorialSuccess->ShowSuccessAnimation);
		}
	}
}

void USMTutorialUIControlComponent::InternalTransitionToGuide()
{
	UE_LOG(LogTemp, Warning, TEXT("가이드로 전환 시작"));

	if (USMTutorialGuide* TutorialGuide = GetTutorialGuide())
	{
		TutorialGuide->SetVisibility(ESlateVisibility::Hidden);
	}

	if (USMTutorialSuccess* TutorialSuccess = GetTutorialSuccess())
	{
		TutorialSuccess->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}

	if (USMTutorialMission* TutorialMission = GetTutorialMission())
	{
		TutorialMission->SetVisibility(ESlateVisibility::Hidden);
	}

	if (USMTutorialSuccess* TutorialSuccess = GetTutorialSuccess())
	{
		if (TutorialSuccess->HideSuccessAnimation)
		{
			FWidgetAnimationDynamicEvent EndDelegate;
			EndDelegate.BindDynamic(this, &ThisClass::OnHideSuccessAnimationEnded);
			TutorialSuccess->UnbindAllFromAnimationFinished(TutorialSuccess->HideSuccessAnimation);
			TutorialSuccess->BindToAnimationFinished(TutorialSuccess->HideSuccessAnimation, EndDelegate);
			TutorialSuccess->PlayAnimation(TutorialSuccess->HideSuccessAnimation);
		}
		else
		{
			OnHideSuccessAnimationEnded();
		}
	}
}

void USMTutorialUIControlComponent::OnHideSuccessAnimationEnded()
{
	UE_LOG(LogTemp, Warning, TEXT("목표 완료 숨기기 애니메이션 종료"));

	if (USMTutorialSuccess* TutorialSuccess = GetTutorialSuccess())
	{
		TutorialSuccess->SetVisibility(ESlateVisibility::Hidden);
	}

	if (USMTutorialGuide* TutorialGuide = GetTutorialGuide())
	{
		TutorialGuide->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

		if (TutorialGuide->ShowGuideAnimation)
		{
			FWidgetAnimationDynamicEvent EndDelegate;
			EndDelegate.BindDynamic(this, &ThisClass::OnShowGuideAnimationEnded);
			TutorialGuide->UnbindAllFromAnimationFinished(TutorialGuide->ShowGuideAnimation);
			TutorialGuide->BindToAnimationFinished(TutorialGuide->ShowGuideAnimation, EndDelegate);
			TutorialGuide->PlayAnimation(TutorialGuide->ShowGuideAnimation);
		}
		else
		{
			OnShowGuideAnimationEnded();
		}
	}
}

void USMTutorialUIControlComponent::OnShowGuideAnimationEnded()
{
	UE_LOG(LogTemp, Warning, TEXT("가이드 보이기 애니메이션 종료"));

	if (USMTutorialMission* TutorialMission = GetTutorialMission())
	{
		TutorialMission->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

		if (TutorialMission->ShowMissionAnimation)
		{
			TutorialMission->PlayAnimation(TutorialMission->ShowMissionAnimation);
		}
	}
}
