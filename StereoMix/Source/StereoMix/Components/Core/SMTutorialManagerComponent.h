// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "Components/ActorComponent.h"
#include "Data/SMCharacterType.h"
#include "SMTutorialManagerComponent.generated.h"

class ALevelSequenceActor;
class ULevelSequencePlayer;
class ULevelSequence;
class UNiagaraComponent;
class UNiagaraSystem;
class ASMAICharacterBase;
class ASMTile;
class ASMTrainingDummy;
class ASMProgressTriggerBase;
class ASMTutorialWall;
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

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void Activate(bool bReset) override;

protected:
	APawn* GetLocalPlayerPawn();

	void TransformScriptsData();

	FString GetScriptText(int32 StepNumber, int32 ScriptNumberInStep, ESMCharacterType CharacterType);

	ESMCharacterType GetLocalPlayerCharacterType();

	UFUNCTION()
	void OnStep1Completed(AActor* OverlappedActor, AActor* OtherActor);

	void OnStep2Started();

	UFUNCTION()
	void OnSamplingTilesCaptured();

	void OnStep2Completed();

	void OnStep3Started();

	void OnStep3Completed();

	void OnStep4Started();

	UFUNCTION()
	void OnStep4Completed();

	void OnStep5Started();

	void OnStep5Completed();

	void OnStep6Started();

	UFUNCTION()
	void OnStep6Completed();

	void OnStep7Started();

	UFUNCTION()
	void OnTileCapturedByNoiseBreak(TArray<ASMTile*> CapturedTiles);

	void RestartStep7();

	UFUNCTION()
	void OnStep7Completed();

	void OnStep8Started();

	void OnHPChanged(const FOnAttributeChangeData& OnAttributeChangeData);

	UFUNCTION()
	void OnStep8Completed();

	void OnStep9Started();

	UFUNCTION()
	void OnNextTriggerBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

	void OnArrivedBattleZone();

	void OnStep9Completed();

	void OnStep10Started();

	UFUNCTION()
	void StartBattle(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION()
	void OnStep10Completed();

	void OnStep11Started();

	UFUNCTION()
	void OnStep11Completed(AActor* OverlappedActor, AActor* OtherActor);

	UPROPERTY(EditAnywhere, Category = "Design")
	TObjectPtr<UDataTable> TutorialScriptDataTable;

	UPROPERTY()
	TObjectPtr<USMTutorialUIControlComponent> CachedTutorialUIControlComponent;

	UPROPERTY()
	TObjectPtr<ACharacter> CachedCharacter;

	/** Step, ScriptNumberInStep, 캐릭터 타입 순서로 접근하면 됩니다. */
	TArray<TArray<TMap<ESMCharacterType, FScriptData>>> UIScripts;

	int32 CurrentStepNumber = 1;

	int32 TilesCaptureCount = 0;

	int32 TargetTilesCaptureCountForStep2 = 9;

	TWeakObjectPtr<ASMTutorialWall> SamplingWall;

	TWeakObjectPtr<ASMTutorialWall> HoldWall;

	TWeakObjectPtr<ASMTutorialWall> NoiseBreakWall;

	TWeakObjectPtr<ASMTutorialWall> HealPackWall;

	TWeakObjectPtr<ASMTutorialWall> NextSectionWall;

	TWeakObjectPtr<ASMTutorialWall> BattleEntranceWall;

	TWeakObjectPtr<ASMTutorialWall> BattleExitWall;

	TWeakObjectPtr<ASMProgressTriggerBase> MovePracticeTrigger;

	TWeakObjectPtr<ASMProgressTriggerBase> NextTrigger;

	TWeakObjectPtr<ASMProgressTriggerBase> EndTrigger;

	TWeakObjectPtr<ASMProgressTriggerBase> BattleStartTrigger;

	TArray<TWeakObjectPtr<ASMTile>> SamplingTiles;

	TArray<TWeakObjectPtr<ASMTile>> NoiseBreakTiles;

	TArray<TWeakObjectPtr<ASMTile>> HealPackTiles;

	TArray<TWeakObjectPtr<ASMTile>> PreNextSectionTiles;

	FVector NoiseBreakRestartLocation;

	float CachedOwnerMaxHP = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Design")
	TSubclassOf<ASMTrainingDummy> TrainingDummyClass;

	TObjectPtr<ASMTrainingDummy> TrainingDummy;

	FVector TrainingDummyLocation;

	UPROPERTY(EditAnywhere, Category = "Design")
	TSubclassOf<ASMAICharacterBase> AICharacterClass;

	UPROPERTY()
	TObjectPtr<ASMAICharacterBase> AICharacter;

	FVector AISpawnLocation;

	UPROPERTY(EditAnywhere, Category = "Design")
	TObjectPtr<UNiagaraSystem> LocationIndicator3X3;

	UPROPERTY()
	TObjectPtr<UNiagaraComponent> LocationIndicator3X3Component;

	UPROPERTY(EditAnywhere, Category = "Design")
	TObjectPtr<UNiagaraSystem> LocationIndicator3X3Clear;

	UPROPERTY()
	TObjectPtr<UNiagaraComponent> LocationIndicator3X3ClearComponent;

	UPROPERTY(EditAnywhere, Category = "Design")
	TObjectPtr<UNiagaraSystem> LocationIndicator5X5;

	UPROPERTY()
	TObjectPtr<UNiagaraComponent> LocationIndicator5X5Component;

	UPROPERTY(EditAnywhere, Category = "Design")
	TObjectPtr<ULevelSequence> VanSequence;

	UPROPERTY(EditAnywhere, Category = "Design")
	TObjectPtr<ULevelSequencePlayer> VanSequencePlayer;
};
