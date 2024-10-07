// Copyright Surround, Inc. All Rights Reserved.


#include "SMGamePlayerController.h"

#include "GameFramework/GameModeBase.h"
#include "AbilitySystemComponent.h"
#include "Blueprint/UserWidget.h"
#include "FunctionLibraries/SMTeamBlueprintLibrary.h"
#include "StereoMixLog.h"
#include "AbilitySystem/AttributeSets/SMCharacterAttributeSet.h"
#include "Actors/Character/Player/SMPlayerCharacterBase.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Games/SMGamePlayerState.h"
#include "UI/Widget/Dummy/SMUserWidget_StaminaSkillGaugeDummyBar.h"
#include "UI/Widget/Game/SMUserWidget_GameStatistics.h"
#include "UI/Widget/Game/SMUserWidget_HUD.h"
#include "UI/Widget/Game/SMUserWidget_StartCountdown.h"
#include "UI/Widget/Game/SMUserWidget_VictoryDefeat.h"
#include "Utilities/SMLog.h"

ASMGamePlayerController::ASMGamePlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Type::Crosshairs;
}

void ASMGamePlayerController::InitPlayerState()
{
	Super::InitPlayerState();

	// 에디터에서 해당 클래스를 상속한 블루프린트를 열 때 오류가 발생하기 때문에 GameMode 인스턴스 존재 여부를 확인함
	if (HasAuthority() && GetWorld()->GetAuthGameMode())
	{
		// 오류를 방지하기위해 지연 스폰합니다.
		auto ThisWeakPtr = MakeWeakObjectPtr(this);
		GetWorldTimerManager().SetTimerForNextTick([ThisWeakPtr] {
			if (ThisWeakPtr.IsValid())
			{
				ThisWeakPtr->SpawnCharacter();
			}
		});
	}
}

void ASMGamePlayerController::BeginPlay()
{
	Super::BeginPlay();

	InitControl();

	if (IsLocalController())
	{
		InitUI();
	}
}

void ASMGamePlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	GameStatisticsUpdateTime += DeltaSeconds;
	if (GameStatisticsUpdateTime >= GameStatisticsUpdateInterval)
	{
		GameStatisticsUpdateTime = 0.f;
		UpdateGameStatistics();
	}
}

void ASMGamePlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	InitUI();
}

void ASMGamePlayerController::InitUI()
{
	const ASMGamePlayerState* SMPlayerState = GetPlayerState<ASMGamePlayerState>();
	UAbilitySystemComponent* SourceASC = SMPlayerState ? SMPlayerState->GetAbilitySystemComponent() : nullptr;
	if (!SourceASC)
	{
		return;
	}

	if (HUDWidget = CreateWidget<USMUserWidget_HUD>(this, HUDWidgetClass); HUDWidget)
	{
		HUDWidget->AddToViewport(0);
		HUDWidget->SetASC(SourceASC);
	}

	if (VictoryDefeatWidget = CreateWidget<USMUserWidget_VictoryDefeat>(this, VictoryDefeatWidgetClass); VictoryDefeatWidget)
	{
		VictoryDefeatWidget->AddToViewport(1);
		VictoryDefeatWidget->SetASC(SourceASC);
	}

	if (StartCountdownWidget = CreateWidget<USMUserWidget_StartCountdown>(this, StartCountdownWidgetClass); StartCountdownWidget)
	{
		StartCountdownWidget->AddToViewport(1);
	}

	if (GameStatisticsWidget = CreateWidget<USMUserWidget_GameStatistics>(this, GameStatisticsWidgetClass); GameStatisticsWidget)
	{
		GameStatisticsWidget->AddToViewport(10);
	}

	if (DummyBarWidget = CreateWidget<USMUserWidget_StaminaSkillGaugeDummyBar>(this, DummyBarWidgetClass); DummyBarWidget)
	{
		DummyBarWidget->AddToViewport(2);

		if (const USMCharacterAttributeSet* AttributeSet = SourceASC->GetSet<USMCharacterAttributeSet>())
		{
			SourceASC->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetStaminaAttribute()).AddUObject(DummyBarWidget, &USMUserWidget_StaminaSkillGaugeDummyBar::OnStaminaChanged);
			SourceASC->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetSkillGaugeAttribute()).AddUObject(DummyBarWidget, &USMUserWidget_StaminaSkillGaugeDummyBar::OnSkillGaugeChanged);

			DummyBarWidget->Stamina = AttributeSet->GetStamina();
			DummyBarWidget->MaxStamina = AttributeSet->GetMaxStamina();
			DummyBarWidget->UpdateStaminaBar();

			DummyBarWidget->SkillGauge = AttributeSet->GetSkillGauge();
			DummyBarWidget->MaxSkillGauge = AttributeSet->GetMaxSkillGauge();
			DummyBarWidget->UpdateSkillGaugeBar();
		}
	}
}

void ASMGamePlayerController::UpdateGameStatistics()
{
	if (!PlayerState || GetNetMode() != NM_Client)
	{
		return;
	}

	if (GameStatisticsWidget)
	{
		// NET_LOG(this, Verbose, TEXT("Ping: %fms"), PlayerState->GetPingInMilliseconds())
		const int32 Ping = FMath::CeilToInt(PlayerState->GetPingInMilliseconds());
		GameStatisticsWidget->UpdatePingText(Ping);
	}
}

void ASMGamePlayerController::SpawnCharacter(const TOptional<FVector>& InLocationOption, const TOptional<FRotator>& InRotationOption)
{
	const ASMGamePlayerState* SMPlayerState = Cast<ASMGamePlayerState>(PlayerState);
	UWorld* World = GetWorld();
	AGameModeBase* GameMode = World ? World->GetAuthGameMode() : nullptr;
	if (!HasAuthority() || !SMPlayerState || !World || !GameMode)
	{
		return;
	}

	ESMCharacterType CharacterType = SMPlayerState->GetCharacterType();
	const ESMTeam SourceTeam = SMPlayerState->GetTeam();

	// 플레이어 스타트를 통해 알맞은 스폰 장소를 설정합니다.
	FString TeamStarterTag;
	switch (SourceTeam)
	{
		case ESMTeam::EDM:
		{
			TeamStarterTag = TEXT("Starter_EDM");
			break;
		}
		case ESMTeam::FutureBass:
		{
			TeamStarterTag = TEXT("Starter_FB");
			break;
		}
		default:
		{
			break;
		}
	}

	// 스폰 위치와 회전을 구합니다.
	const AActor* PlayerStarter = GameMode->FindPlayerStart(this, TeamStarterTag);
	const FVector CharacterSpawnLocation = InLocationOption.Get(PlayerStarter ? PlayerStarter->GetActorLocation() : FVector::ZeroVector);
	const FRotator CharacterSpawnRotation = InRotationOption.Get(PlayerStarter ? PlayerStarter->GetActorRotation() : FRotator::ZeroRotator);

	// PlayerState에 캐릭터 타입이 지정되어있지 않다면 플레이어 컨트롤러에 설정된 기본 캐릭터 타입으로 덮어 씌웁니다. 테스트 환경을 위한 코드입니다.
	CharacterType = (CharacterType == ESMCharacterType::None) ? DefaultType : CharacterType;

	FCharacterSpawnData* CharacterSpawnDataPtr = CharacterClass.Find(CharacterType);
	const TSubclassOf<ASMPlayerCharacterBase>* CharacterClassToSpawnPtr = CharacterSpawnDataPtr ? CharacterSpawnDataPtr->CharacterClass.Find(SourceTeam) : nullptr;
	const TSubclassOf<ASMPlayerCharacterBase>& CharacterClassToSpawn = CharacterClassToSpawnPtr ? *CharacterClassToSpawnPtr : nullptr;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	if (ASMPlayerCharacterBase* PlayerCharacter = World->SpawnActor<ASMPlayerCharacterBase>(CharacterClassToSpawn, CharacterSpawnLocation, CharacterSpawnRotation, SpawnParams))
	{
		// 기존 캐릭터를 제거합니다.
		if (APawn* PreviousPawn = GetPawn())
		{
			PreviousPawn->Destroy();
		}

		Possess(PlayerCharacter);
	}
}

void ASMGamePlayerController::RequestImmediateResetPosition_Implementation()
{
#if WITH_SERVER_CODE
	ACharacter* PlayerCharacter = GetCharacter();
	if (!PlayerCharacter)
	{
		return;
	}

	const ASMGamePlayerState* SMPlayerState = Cast<ASMGamePlayerState>(PlayerState);
	if (!SMPlayerState)
	{
		return;
	}

	const ESMTeam Team = SMPlayerState->GetTeam();

	// 플레이어 스타트를 통해 알맞은 스폰 장소를 설정합니다.
	FVector NewLocation;
	FRotator NewRotation;
	if (AGameModeBase* GameMode = GetWorld()->GetAuthGameMode(); ensureAlways(GameMode))
	{
		FString TeamStarterTag;
		switch (Team)
		{
			case ESMTeam::EDM:
			{
				TeamStarterTag = TEXT("Starter_EDM");
				break;
			}
			case ESMTeam::FutureBass:
			{
				TeamStarterTag = TEXT("Starter_FB");
				break;
			}
			default:
			{
				break;
			}
		}

		if (const AActor* PlayerStarter = GameMode->FindPlayerStart(this, TeamStarterTag); ensureAlways(PlayerStarter))
		{
			NewLocation = PlayerStarter->GetActorLocation();
			NewRotation = PlayerStarter->GetActorRotation();
		}
	}

	PlayerCharacter->TeleportTo(NewLocation, NewRotation);
#endif
}
