// Copyright Surround, Inc. All Rights Reserved.


#include "SMGamePlayerController.h"

#include "AbilitySystemComponent.h"
#include "EnhancedInputSubsystems.h"
#include "SMGamePlayerState.h"
#include "StereoMixLog.h"
#include "Blueprint/UserWidget.h"
#include "Characters/SMPlayerCharacter.h"
#include "Data/SMControlData.h"
#include "FunctionLibraries/SMTeamBlueprintLibrary.h"
#include "GameFramework/GameModeBase.h"
#include "Games/SMGameMode.h"
#include "UI/Widget/Game/SMUserWidget_GameStatistics.h"
#include "UI/Widget/Game/SMUserWidget_HUD.h"
#include "UI/Widget/Game/SMUserWidget_ScreenIndicator.h"
#include "UI/Widget/Game/SMUserWidget_StartCountdown.h"
#include "UI/Widget/Game/SMUserWidget_VictoryDefeat.h"
#include "Utilities/SMAssetPath.h"
#include "Utilities/SMLog.h"

ASMGamePlayerController::ASMGamePlayerController()
{
	static ConstructorHelpers::FObjectFinder<USMControlData> ControlDataRef(SMAssetPath::ControlData);
	if (ControlDataRef.Object)
	{
		ControlData = ControlDataRef.Object;
	}
	else
	{
		UE_LOG(LogLoad, Error, TEXT("ControlData 로드에 실패했습니다."));
	}

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
		GetWorldTimerManager().SetTimerForNextTick(this, &ThisClass::SpawnTimerCallback);
	}
}

void ASMGamePlayerController::BeginPlay()
{
	Super::BeginPlay();

	InitControl();
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

void ASMGamePlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (HasAuthority())
	{
		FString EndPlayReasonString = UEnum::GetValueAsString(EndPlayReason);
		NET_LOG(this, Log, TEXT("%s"), *EndPlayReasonString);
	}

	Super::EndPlay(EndPlayReason);
}

void ASMGamePlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	ASMGamePlayerState* SMPlayerState = GetPlayerState<ASMGamePlayerState>();
	if (!SMPlayerState)
	{
		return;
	}

	HUDWidget = CreateWidget<USMUserWidget_HUD>(this, HUDWidgetClass);
	HUDWidget->AddToViewport(0);

	VictoryDefeatWidget = CreateWidget<USMUserWidget_VictoryDefeat>(this, VictoryDefeatWidgetClass);
	VictoryDefeatWidget->AddToViewport(1);

	UAbilitySystemComponent* SourceASC = SMPlayerState->GetAbilitySystemComponent();
	HUDWidget->SetASC(SourceASC);
	VictoryDefeatWidget->SetASC(SourceASC);

	StartCountdownWidget = CreateWidget<USMUserWidget_StartCountdown>(this, StartCountdownWidgetClass);
	StartCountdownWidget->AddToViewport(1);

	GameStatisticsWidget = CreateWidget<USMUserWidget_GameStatistics>(this, GameStatisticsWidgetClass);
	GameStatisticsWidget->AddToViewport(10);
}

void ASMGamePlayerController::InitControl()
{
	if (!IsLocalController())
	{
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (Subsystem)
	{
		Subsystem->ClearAllMappings();
		Subsystem->AddMappingContext(ControlData->DefaultMappingContext, 0);
	}

	// FInputModeGameOnly InputModeGameOnly;
	// InputModeGameOnly.SetConsumeCaptureMouseDown(true);
	// SetInputMode(InputModeGameOnly);
}

void ASMGamePlayerController::SpawnTimerCallback()
{
	AGameModeBase* GameMode = GetWorld()->GetAuthGameMode();
	if (!GameMode)
	{
		NET_LOG(this, Error, TEXT("게임모드가 유효하지 않습니다."));
		return;
	}

	// 스폰 포인트를 구합니다. 여기서는 SpawnPoint라는 태그를 가진 플레이어 스타트를 활용하고 있습니다.
	// AActor* PlayerStarter = GameMode->FindPlayerStart(this, TEXT("SpawnPoint"));
	// if (!ensureAlways(PlayerStarter))
	// {
	// 	return;
	// }
	//
	// const FVector NewLocation = PlayerStarter->GetActorLocation();
	SpawnCharacter();
}

void ASMGamePlayerController::UpdateGameStatistics()
{
	if (!PlayerState || GetNetMode() != NM_Client)
	{
		return;
	}

	if (GameStatisticsWidget)
	{
		GameStatisticsWidget->UpdatePingText(PlayerState->GetCompressedPing());
	}
}

void ASMGamePlayerController::SpawnCharacter(const FVector* InLocation, const FRotator* InRotation)
{
	if (!HasAuthority())
	{
		NET_LOG(this, Warning, TEXT("서버에서만 호출되어야합니다."));
		return;
	}

	ASMGamePlayerState* SMPlayerState = Cast<ASMGamePlayerState>(PlayerState);
	if (!ensureAlways(SMPlayerState))
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!ensureAlways(World))
	{
		return;
	}

	ESMCharacterType CharacterType = SMPlayerState->GetCharacterType();
	ESMTeam Team = SMPlayerState->GetTeam();

	// 플레이어 스타트를 통해 알맞은 스폰 장소를 설정합니다.
	FVector NewLocation;
	FRotator NewRotation;
	AGameModeBase* GameMode = GetWorld()->GetAuthGameMode();
	if (ensureAlways(GameMode))
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

		AActor* PlayerStarter = GameMode->FindPlayerStart(this, TeamStarterTag);
		if (ensureAlways(PlayerStarter))
		{
			NewLocation = PlayerStarter->GetActorLocation();
			NewRotation = PlayerStarter->GetActorRotation();
		}
	}

	// 만약 따로 위치나 회전 값이 매개변수로 주어졌다면 이 값으로 덮어 씌웁니다.
	if (InLocation)
	{
		NewLocation = *InLocation;
	}
	if (InRotation)
	{
		NewRotation = *InRotation;
	}

	// 임시 코드
	if (CharacterType == ESMCharacterType::None)
	{
		UE_LOG(LogStereoMix, Warning, TEXT("캐릭터 타입이 None으로 설정되어있습니다. 기본값으로 설정합니다."))
		CharacterType = ESMCharacterType::ElectricGuitar;
	}

	FCharacterSpawnData* CharacterSpawnData = CharacterClass.Find(CharacterType);
	if (!ensureAlways(CharacterSpawnData))
	{
		return;
	}

	TSubclassOf<ASMPlayerCharacter> CharacterSpawnClass = CharacterSpawnData->CharacterClass.FindOrAdd(Team, nullptr);
	if (!ensureAlways(CharacterSpawnClass))
	{
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ASMPlayerCharacter* PlayerCharacter = World->SpawnActor<ASMPlayerCharacter>(CharacterSpawnClass, NewLocation, NewRotation, SpawnParams);
	if (!ensureAlways(PlayerCharacter))
	{
		return;
	}

	// 기존 캐릭터를 제거합니다.  
	ASMPlayerCharacter* PreviousCharacter = GetPawn<ASMPlayerCharacter>();
	if (PreviousCharacter)
	{
		PreviousCharacter->Destroy();
	}

	Possess(PlayerCharacter);
}

void ASMGamePlayerController::AddScreendIndicator(AActor* TargetActor)
{
	if (HasAuthority())
	{
		return;
	}

	// 플레이어 캐릭터를 가진 경우에만 스크린 인디케이터를 추가합니다. 
	if (!Cast<ASMPlayerCharacter>(GetPawn()))
	{
		return;
	}

	// 같은 팀인 경우 인디케이터를 생성하지 않습니다.
	if (USMTeamBlueprintLibrary::IsSameTeam(TargetActor))
	{
		return;
	}

	// 인디케이터를 생성하고 타겟을 지정합니다.
	USMUserWidget_ScreenIndicator* OffScreendIndicator = CreateWidget<USMUserWidget_ScreenIndicator>(this, OffScreenIndicatorClass);
	OffScreendIndicator->AddToViewport(-1);
	OffScreendIndicator->SetTarget(TargetActor);
	OffScreenIndicators.Add(TargetActor, OffScreendIndicator);

	// 대상이 게임에서 나갔을 경우의 예외처리입니다.
	TargetActor->OnDestroyed.AddDynamic(this, &ThisClass::OnTargetDestroyedWithIndicator);
}

void ASMGamePlayerController::RemoveScreenIndicator(AActor* TargetActor)
{
	if (HasAuthority())
	{
		return;
	}

	// 플레이어 캐릭터를 가진 경우에만 스크린 인디케이터를 제거합니다. 
	if (!Cast<ASMPlayerCharacter>(GetPawn()))
	{
		return;
	}

	// 인디케이터를 제거합니다. 중복 제거되도 문제는 없습니다.
	TObjectPtr<USMUserWidget_ScreenIndicator>* OffScreendIndicator = OffScreenIndicators.Find(TargetActor);
	if (!OffScreendIndicator)
	{
		return;
	}

	(*OffScreendIndicator)->RemoveFromParent();
	(*OffScreendIndicator)->ConditionalBeginDestroy();
	OffScreenIndicators.Remove(TargetActor);

	// 대상이 게임에서 나갔을 경우의 예외처리로 사용된 이벤트를 제거합니다.
	TargetActor->OnDestroyed.RemoveAll(this);
}

void ASMGamePlayerController::OnTargetDestroyedWithIndicator(AActor* DestroyedActor)
{
	// 대상이 게임에서 나가거나 어떤 이유로 유효하지 않게된 경우 인디케이터를 제거해줍니다.
	NET_LOG(this, Warning, TEXT("유효하지 않은 타겟의 인디케이터 제거"));
	RemoveScreenIndicator(DestroyedActor);
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
