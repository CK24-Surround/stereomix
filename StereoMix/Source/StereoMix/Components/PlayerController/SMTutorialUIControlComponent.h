// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SMTutorialUIControlComponent.generated.h"


class USMTutorialHUD;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STEREOMIX_API USMTutorialUIControlComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USMTutorialUIControlComponent();

	virtual void Activate(bool bReset) override;

	USMTutorialHUD* GetTutorialHUD() const { return TutorialHUD; }

	void SetScript(const FString& InString);

protected:
	UFUNCTION()
	void OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn);

	UPROPERTY(EditAnywhere, Category = "Design")
	TSubclassOf<USMTutorialHUD> TutorialHUDClass;

	UPROPERTY()
	TObjectPtr<USMTutorialHUD> TutorialHUD;
};
