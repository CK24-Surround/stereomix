// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SMTutorialUIControlComponent.generated.h"

DECLARE_DELEGATE(FOnWidgetTransitionEnded);

class USMTutorialSuccess;
class USMTutorialMission;
class USMTutorialGuide;
class USMTutorialHUD;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STEREOMIX_API USMTutorialUIControlComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USMTutorialUIControlComponent();

	virtual void Activate(bool bReset) override;

	USMTutorialHUD* GetTutorialHUD() const { return TutorialHUD; }

	USMTutorialGuide* GetTutorialGuide() const;

	USMTutorialSuccess* GetTutorialSuccess() const;

	USMTutorialMission* GetTutorialMission() const;

	void SetGuideText(const FString& InString);

	void SetMissionText(const FString& InString);

	void ShowGuide();

	void ShowMission();

	void ShowSuccess();

	void TransitionToSuccess();

	void TransitionToGuide();

	void TransitionAndSetText(const FString& InGuideText, const FString& InMissionText, float CompletionDisplayTime);

	FOnWidgetTransitionEnded OnTransitionAndSetTextEnded;

protected:
	void InternalTransitionToSuccess();

	UFUNCTION()
	void OnHideMissionAnimationEnded();

	UFUNCTION()
	void OnHideGuideAnimationEnded();

	UFUNCTION()
	void OnShowSuccessAnimationEnded();

	void InternalTransitionToGuide();

	UFUNCTION()
	void OnHideSuccessAnimationEnded();

	UFUNCTION()
	void OnShowGuideAnimationEnded();

	UFUNCTION()
	void OnShowMissionAnimationEnded();

	void InternalTransitionAndSetText(const FString& InGuideText, const FString& InMissionText, float CompletionDisplayTime);

	void OnTransitionAndSetTextEndedCallback();

	FOnWidgetTransitionEnded OnTransitionToSuccessEnded;

	FOnWidgetTransitionEnded OnTransitionToGuideEnded;

	FString GuideText;

	FString MissionText;

	UPROPERTY(EditAnywhere, Category = "Design")
	TSubclassOf<USMTutorialHUD> TutorialHUDClass;

	UPROPERTY()
	TObjectPtr<USMTutorialHUD> TutorialHUD;
};
