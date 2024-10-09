// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SMTutorialManagerComponent.generated.h"

class UEnhancedInputComponent;
class UInputAction;
class USMTutorialHUD;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STEREOMIX_API USMTutorialManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USMTutorialManagerComponent();

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void Activate(bool bReset) override;

protected:
	UFUNCTION()
	void OnProgressTriggerBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

	void OnNextInputReceived();

	UPROPERTY(EditAnywhere, Category = "Design|IA")
	TObjectPtr<UInputAction> NextInputAction;

	TArray<FString> Scripts;
};
