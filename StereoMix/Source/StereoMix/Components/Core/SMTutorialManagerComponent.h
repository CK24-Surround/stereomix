// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/SMCharacterType.h"
#include "SMTutorialManagerComponent.generated.h"

class USMTutorialUIControlComponent;
class UEnhancedInputComponent;
class UInputAction;
class USMTutorialHUD;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STEREOMIX_API USMTutorialManagerComponent : public UActorComponent
{
	GENERATED_BODY()

	struct FScriptData
	{
		FString Ko;
		FString En;
		FString Ja;
	};

public:
	USMTutorialManagerComponent();

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void Activate(bool bReset) override;

protected:
	UFUNCTION()
	void OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn);

	void TransformScriptsData();

	void OnNextInputReceived();

	UFUNCTION()
	void OnProgressTriggerBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

	void PrintScript(int32 StepNumber, int32 ScriptsNumber);

	void OnScriptsEnded();

	void OnStep1Ended();

	void OnStep2Ended();

	void OnStep3Ended();

	void OnStep4Ended();

	void OnStep5Ended();

	UFUNCTION()
	void OnTilesCaptured(const AActor* CapturedInstigator, int32 CapturedTileCount);

	UPROPERTY(EditAnywhere, Category = "Design|IA")
	TObjectPtr<UInputAction> NextInputAction;

	UPROPERTY(EditAnywhere, Category = "Design")
	TObjectPtr<UDataTable> TutorialScriptDataTable;

	UPROPERTY()
	TObjectPtr<USMTutorialUIControlComponent> CachedTutorialUIControlComponent;

	/** Step, ScriptNumberInStep, 캐릭터 타입 순서로 접근하면 됩니다. */
	TArray<TArray<TMap<ESMCharacterType, FScriptData>>> DialogueScripts;

	/** Step, ScriptNumberInStep, 캐릭터 타입 순서로 접근하면 됩니다. */
	TArray<TArray<TMap<ESMCharacterType, FScriptData>>> UIScripts;

	int32 CurrentStepNumber = 1;

	int32 CurrentScriptNumber = 1;

	int32 TilesCaptureCount = 0;
};
