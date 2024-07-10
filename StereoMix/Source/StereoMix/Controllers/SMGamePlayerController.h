// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SMPlayerController.h"
#include "Data/SMCharacterType.h"
#include "Data/SMTeam.h"

#include "SMGamePlayerController.generated.h"

class USMUserWidget_HUD;
class USMUserWidget_StartCountdown;
class ASMPlayerCharacter;
class USMUserWidget_ScreenIndicator;
class USMUserWidget_VictoryDefeat;
class USMUserWidget_GameHUD;
class USMUserWidget_GameStatistics;
class USMControlData;

USTRUCT(BlueprintType)
struct FCharacterSpawnData
{
	GENERATED_BODY()

	FCharacterSpawnData()
	{
	}

	UPROPERTY(EditAnywhere)
	TMap<ESMTeam, TSubclassOf<ASMPlayerCharacter>> CharacterClass;
};

/**
 *
 */
UCLASS()
class STEREOMIX_API ASMGamePlayerController : public ASMPlayerController
{
	GENERATED_BODY()

public:
	ASMGamePlayerController();

	virtual void InitPlayerState() override;

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void OnRep_PlayerState() override;

	/**
	 * BeginPlay에서 생성할때 오류를 방지하고자 지연 스폰합니다.
	 */
	void SpawnTimerCallback();

	virtual void UpdateGameStatistics();

public:
	/**
	 * 캐릭터를 스폰하고 빙의합니다. 이미 캐릭터가 존재하는 경우라면 해당 캐릭터를 제거하고 새로운 캐릭터를 스폰하고 빙의합니다. 이때 기존 캐릭터로부터 부여된 어빌리티들을 초기화합니다.
	 * 위치 값을 넣어주지 않으면 내부 로직에 의해 적합한 스폰장소에 스폰합니다.
	 */
	void SpawnCharacter(const FVector* InLocation = nullptr, const FRotator* InRotation = nullptr);

	/** 타겟을 향하는 인디케이터를 추가합니다. */
	void AddScreendIndicator(AActor* TargetActor);

	/** 타겟을 향하는 인디케이터를 제거합니다. */
	void RemoveScreenIndicator(AActor* TargetActor);

	/** 인디케이터를 갖고 있는 타겟이 파괴되면 호출됩니다.*/
	UFUNCTION()
	void OnTargetDestroyedWithIndicator(AActor* DestroyedActor);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void RequestImmediateResetPosition();

protected:
	UPROPERTY(EditAnywhere, Category = "GameStatistics")
	float GameStatisticsUpdateInterval = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Design|Character|Class")
	TMap<ESMCharacterType, FCharacterSpawnData> CharacterClass;

	UPROPERTY(EditAnywhere, Category = "Design|Character")
	ESMCharacterType DefaultType = ESMCharacterType::None;

	UPROPERTY(EditAnywhere, Category = "Design|UI|HUD")
	TSubclassOf<USMUserWidget_HUD> HUDWidgetClass;

	UPROPERTY()
	TObjectPtr<USMUserWidget_HUD> HUDWidget;

	UPROPERTY(EditAnywhere, Category = "Design|UI|HUD")
	TSubclassOf<USMUserWidget_VictoryDefeat> VictoryDefeatWidgetClass;

	UPROPERTY()
	TObjectPtr<USMUserWidget_VictoryDefeat> VictoryDefeatWidget;

	UPROPERTY(EditAnywhere, Category = "Design|UI|OffScreenIndicator")
	TSubclassOf<USMUserWidget_ScreenIndicator> OffScreenIndicatorClass;

	UPROPERTY()
	TMap<AActor*, TObjectPtr<USMUserWidget_ScreenIndicator>> OffScreenIndicators;

	UPROPERTY(EditAnywhere, Category = "Design|UI|StartCountdown")
	TSubclassOf<USMUserWidget_StartCountdown> StartCountdownWidgetClass;

	UPROPERTY()
	TObjectPtr<USMUserWidget_StartCountdown> StartCountdownWidget;

	UPROPERTY(EditDefaultsOnly, Category = "Design|UI|GameStatistics")
	TSubclassOf<USMUserWidget_GameStatistics> GameStatisticsWidgetClass;

	UPROPERTY()
	TObjectPtr<USMUserWidget_GameStatistics> GameStatisticsWidget;

private:
	float GameStatisticsUpdateTime = 0.f;
};
