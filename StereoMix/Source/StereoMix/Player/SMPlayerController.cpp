// Copyright Surround, Inc. All Rights Reserved.


#include "SMPlayerController.h"

#include "AbilitySystemComponent.h"
#include "EnhancedInputSubsystems.h"
#include "SMGamePlayerState.h"
#include "Blueprint/UserWidget.h"
#include "Characters/SMPlayerCharacter.h"
#include "Data/SMControlData.h"
#include "GameFramework/GameModeBase.h"
#include "UI/SMUserWidget_HUD.h"
#include "UI/SMUserWidget_ScreenIndicator.h"
#include "UI/SMUserWidget_VictoryDefeat.h"
#include "Utilities/SMAssetPath.h"
#include "Utilities/SMLog.h"

ASMPlayerController::ASMPlayerController()
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
}

void ASMPlayerController::BeginPlay()
{
	Super::BeginPlay();

	InitControl();

	if (HasAuthority())
	{
		// TODO: 추후 팀 및 캐릭터 선택 UI가 생기면 아래 로직을 제거해야합니다. 
		AGameModeBase* GameMode = GetWorld()->GetAuthGameMode();
		if (ensureAlways(GameMode))
		{
			AActor* PlayerStarter = GameMode->FindPlayerStart(this, TEXT("SpawnPoint"));
			if (ensureAlways(PlayerStarter))
			{
				const FVector NewLocation = PlayerStarter->GetActorLocation();
				SpawnCharacter(&NewLocation);
			}
		}
	}
}

void ASMPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	HUDWidget = CreateWidget<USMUserWidget_HUD>(this, HUDWidgetClass);
	HUDWidget->AddToViewport(0);

	VictoryDefeatWidget = CreateWidget<USMUserWidget_VictoryDefeat>(this, VictoryDefeatWidgetClass);
	VictoryDefeatWidget->AddToViewport(1);

	ASMGamePlayerState* SMPlayerState = GetPlayerState<ASMGamePlayerState>();
	if (ensure(SMPlayerState))
	{
		UAbilitySystemComponent* SourceASC = SMPlayerState->GetAbilitySystemComponent();
		HUDWidget->SetASC(SourceASC);
		VictoryDefeatWidget->SetASC(SourceASC);
	}
}

void ASMPlayerController::InitControl()
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

	FInputModeGameOnly InputModeGameOnly;
	InputModeGameOnly.SetConsumeCaptureMouseDown(false);
	// SetInputMode(InputModeGameOnly);
}

void ASMPlayerController::SpawnCharacter(const FVector* InLocation, const FRotator* InRotation)
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

	// 기존 캐릭터를 제거하고 부여된 어빌리티를 초기화합니다.  
	ASMPlayerCharacter* PreviousCharacter = GetPawn<ASMPlayerCharacter>();
	if (PreviousCharacter)
	{
		UAbilitySystemComponent* SourceASC = SMPlayerState->GetAbilitySystemComponent();
		if (SourceASC)
		{
			SourceASC->ClearAllAbilities();
		}

		PreviousCharacter->Destroy();
	}

	Possess(PlayerCharacter);
}

void ASMPlayerController::AddScreendIndicator(AActor* TargetActor)
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

	// 인디케이터를 생성하고 타겟을 지정합니다.
	USMUserWidget_ScreenIndicator* OffScreendIndicator = CreateWidget<USMUserWidget_ScreenIndicator>(this, OffScreenIndicatorClass);
	OffScreendIndicator->AddToViewport(-1);
	OffScreendIndicator->SetTarget(TargetActor);
	OffScreenIndicators.Add(TargetActor, OffScreendIndicator);
}

void ASMPlayerController::RemoveScreenIndicator(AActor* TargetActor)
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
}
