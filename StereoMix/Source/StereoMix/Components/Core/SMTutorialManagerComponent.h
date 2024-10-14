// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/SMCharacterType.h"
#include "SMTutorialManagerComponent.generated.h"

class ASMProgressTriggerBase;
class ASMTutorialInvisibleWallBase;
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

	virtual void InitializeComponent() override;

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void Activate(bool bReset) override;

	APawn* GetLocalPlayerPawn();

protected:
	UFUNCTION()
	void OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn);

	void TransformScriptsData();

	void OnNextInputReceived();

	UFUNCTION()
	void OnStep1Completed(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION()
	void OnTilesCaptured(const AActor* CapturedInstigator, int32 CapturedTileCount);

	void OnStep2Completed();

	void OnStep3Completed();

	UFUNCTION()
	void OnStep4Completed();

	void OnStep5Completed();

	UFUNCTION()
	void OnStep6Completed();

	UFUNCTION()
	void OnStep7Completed();

	UFUNCTION()
	void OnStep8Completed();

	void OnStep9Completed();

	UFUNCTION()
	void OnStep10Completed(AActor* OverlappedActor, AActor* OtherActor);

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

	int32 CurrentScriptNumber = 0;

	int32 TilesCaptureCount = 0;

	int32 TargetTilesCaptureCountForStep2 = 9;

	int32 TargetTilesCaptureCountForStep9 = 100;

	TWeakObjectPtr<ASMTutorialInvisibleWallBase> SamplingEventWall;

	TWeakObjectPtr<ASMTutorialInvisibleWallBase> NeutralizeEventWall;

	TWeakObjectPtr<ASMTutorialInvisibleWallBase> NoiseBreakEventWall;

	TWeakObjectPtr<ASMTutorialInvisibleWallBase> BattleStartEventWall;

	TWeakObjectPtr<ASMTutorialInvisibleWallBase> BattleEndEventWall;

	TWeakObjectPtr<ASMProgressTriggerBase> MovePracticeTrigger;

	TWeakObjectPtr<ASMProgressTriggerBase> NextTrigger;

	TWeakObjectPtr<ASMProgressTriggerBase> EndTrigger;
};
