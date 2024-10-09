// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/SMCharacterType.h"
#include "SMTutorialManagerComponent.generated.h"

class UEnhancedInputComponent;
class UInputAction;
class USMTutorialHUD;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STEREOMIX_API USMTutorialManagerComponent : public UActorComponent
{
	GENERATED_BODY()

	struct FScriptData
	{
		ESMCharacterType PlayerCharacterType = ESMCharacterType::None;
		FString Ko;
		FString En;
	};

public:
	USMTutorialManagerComponent();

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void Activate(bool bReset) override;

protected:
	void TransformScriptsData();

	void OnNextInputReceived();

	UFUNCTION()
	void OnProgressTriggerBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

	UPROPERTY(EditAnywhere, Category = "Design|IA")
	TObjectPtr<UInputAction> NextInputAction;

	UPROPERTY(EditAnywhere, Category = "Design")
	TObjectPtr<UDataTable> TutorialScriptDataTable;

	/** Step, ScriptNumberInStep 순으로 접근하면 됩니다. */
	TArray<TArray<FScriptData>> DialogueScripts;

	/** Step, ScriptNumberInStep 순으로 접근하면 됩니다. */
	TArray<TArray<FScriptData>> UIScripts;

	TArray<FString> Scripts;

	int32 TutorialStep = 0;

	int32 ScriptIndex = 1;
};
