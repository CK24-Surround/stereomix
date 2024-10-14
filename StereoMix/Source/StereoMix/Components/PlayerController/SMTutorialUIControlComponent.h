// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SMTutorialUIControlComponent.generated.h"

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

protected:
	void InternalTransitionToSuccess();

	UFUNCTION()
	void OnHideMissionAnimationEnded();

	UFUNCTION()
	void OnHideGuideAnimationEnded();

	void InternalTransitionToGuide();

	UFUNCTION()
	void OnHideSuccessAnimationEnded();

	UFUNCTION()
	void OnShowGuideAnimationEnded();

	UPROPERTY(EditAnywhere, Category = "Design")
	TSubclassOf<USMTutorialHUD> TutorialHUDClass;

	UPROPERTY()
	TObjectPtr<USMTutorialHUD> TutorialHUD;
};
