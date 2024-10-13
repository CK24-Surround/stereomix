// Copyright Studio Surround. All Rights Reserved.


#include "SMScoreManagerComponent.h"

#include "GameFramework/GameStateBase.h"
#include "SMTileManagerComponent.h"
#include "StereoMixLog.h"
#include "Actors/Character/Player/SMPlayerCharacterBase.h"
#include "Games/SMGameState.h"
#include "Games/SMPlayerState.h"


void USMScoreManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	const UWorld* World = GetWorld();
	AGameStateBase* GameState = World ? World->GetGameState() : nullptr;

	if (ASMGameState* SMGameState = GameState ? Cast<ASMGameState>(GameState) : nullptr)
	{
		SMGameState->OnPostRoundStart.AddDynamic(this, &ThisClass::LogAllPlayerData);
	}

	if (USMTileManagerComponent* TileManager = GameState ? GameState->GetComponentByClass<USMTileManagerComponent>() : nullptr)
	{
		TileManager->OnTilesCaptured.AddDynamic(this, &ThisClass::AddTotalCapturedTiles);
	}
}

void USMScoreManagerComponent::AddTotalCapturedTiles(const AActor* CapturedInstigator, int32 CapturedTilesCount)
{
	if (!CapturedInstigator)
	{
		return;
	}

	TWeakObjectPtr<const AActor> TargetWeakPtr = MakeWeakObjectPtr(CapturedInstigator);
	PlayerScoreData.FindOrAdd(TargetWeakPtr, GetDefaultPlayerScoreData(TargetWeakPtr)).TotalCapturedTiles += CapturedTilesCount;

	LogPlayerData(TargetWeakPtr);
}

void USMScoreManagerComponent::AddTotalDamageDealt(const AActor* TargetPlayer, float DamageDealt)
{
	if (!TargetPlayer)
	{
		return;
	}

	TWeakObjectPtr<const AActor> TargetWeakPtr = MakeWeakObjectPtr(TargetPlayer);
	PlayerScoreData.FindOrAdd(TargetWeakPtr, GetDefaultPlayerScoreData(TargetWeakPtr)).TotalDamageDealt += DamageDealt;

	LogPlayerData(TargetWeakPtr);
}

void USMScoreManagerComponent::AddTotalDamageReceived(const AActor* TargetPlayer, float DamageReceived)
{
	if (!TargetPlayer)
	{
		return;
	}

	TWeakObjectPtr<const AActor> TargetWeakPtr = MakeWeakObjectPtr(TargetPlayer);
	PlayerScoreData.FindOrAdd(TargetWeakPtr, GetDefaultPlayerScoreData(TargetWeakPtr)).TotalDamageReceived += DamageReceived;

	LogPlayerData(TargetWeakPtr);
}

void USMScoreManagerComponent::AddTotalDeathCount(const AActor* TargetPlayer)
{
	if (!TargetPlayer)
	{
		return;
	}

	TWeakObjectPtr<const AActor> TargetWeakPtr = MakeWeakObjectPtr(TargetPlayer);
	PlayerScoreData.FindOrAdd(TargetWeakPtr, GetDefaultPlayerScoreData(TargetWeakPtr)).TotalDeathCount += 1;

	LogPlayerData(TargetWeakPtr);
}

void USMScoreManagerComponent::AddTotalKillCount(const AActor* TargetPlayer)
{
	if (!TargetPlayer)
	{
		return;
	}

	TWeakObjectPtr<const AActor> TargetWeakPtr = MakeWeakObjectPtr(TargetPlayer);
	PlayerScoreData.FindOrAdd(TargetWeakPtr, GetDefaultPlayerScoreData(TargetWeakPtr)).TotalKillCount += 1;

	LogPlayerData(TargetWeakPtr);
}

void USMScoreManagerComponent::AddTotalNoiseBreakUsage(const AActor* TargetPlayer)
{
	if (!TargetPlayer)
	{
		return;
	}

	TWeakObjectPtr<const AActor> TargetWeakPtr = MakeWeakObjectPtr(TargetPlayer);
	PlayerScoreData.FindOrAdd(TargetWeakPtr, GetDefaultPlayerScoreData(TargetWeakPtr)).TotalNoiseBreakUsage += 1;

	LogPlayerData(TargetWeakPtr);
}

TWeakObjectPtr<const AActor> USMScoreManagerComponent::GetMVPPlayer(ESMTeam Team) const
{
	TWeakObjectPtr<const AActor> MVPPlayer = nullptr;
	int32 MaxScore = 0;

	for (auto& [PlayerActor, ScoreData] : PlayerScoreData)
	{
		const ASMPlayerCharacterBase* Player = Cast<ASMPlayerCharacterBase>(PlayerActor.Get());
		if (!Player || Player->GetTeam() != Team)
		{
			continue;
		}

		if (ScoreData.TotalScore() > MaxScore)
		{
			MaxScore = ScoreData.TotalScore();
			MVPPlayer = PlayerActor;
		}
	}

	return MVPPlayer;
}

void USMScoreManagerComponent::LogAllPlayerData()
{
	for (auto& [PlayerActor, ScoreData] : PlayerScoreData)
	{
		LogPlayerData(PlayerActor);
	}
}

FPlayerScoreData USMScoreManagerComponent::GetDefaultPlayerScoreData(const TWeakObjectPtr<const AActor>& TargetPlayer)
{
	FPlayerScoreData DefaultPlayerScoreData;

	if (!TargetPlayer.IsValid())
	{
		return DefaultPlayerScoreData;
	}

	const ASMPlayerCharacterBase* TargetPlayerCharacter = Cast<ASMPlayerCharacterBase>(TargetPlayer.Get());

	const APawn* TargetPawn = Cast<APawn>(TargetPlayer.Get());
	AController* TargetController = TargetPawn ? TargetPawn->GetController() : nullptr;
	ASMPlayerState* TargetPlayerState = TargetController ? TargetController->GetPlayerState<ASMPlayerState>() : nullptr;

	ESMTeam TargetPlayerTeam = TargetPlayerState ? TargetPlayerState->GetTeam() : ESMTeam::None;
	ESMCharacterType TargetPlayerType = TargetPlayerCharacter ? TargetPlayerCharacter->GetCharacterType() : ESMCharacterType::None;
	FString TargetPlayerName = TargetPlayerState ? TargetPlayerState->GetPlayerName() : FString("None");

	DefaultPlayerScoreData.PlayerTeam = TargetPlayerTeam;
	DefaultPlayerScoreData.CharacterType = TargetPlayerType;
	DefaultPlayerScoreData.PlayerName = TargetPlayerName;

	return DefaultPlayerScoreData;
}

void USMScoreManagerComponent::LogPlayerData(const TWeakObjectPtr<const AActor>& TargetPlayer) const
{
	TMap<ESMTeam, FString> PlayerTeamMap = {
		{ ESMTeam::None, "None" },
		{ ESMTeam::FutureBass, "FutureBass" },
		{ ESMTeam::EDM, "EDM" }
	};
	FString* TargetPlayerTeam = PlayerTeamMap.Find(PlayerScoreData[TargetPlayer].PlayerTeam);

	TMap<ESMCharacterType, FString> PlayerTypeMap = {
		{ ESMCharacterType::None, "None" },
		{ ESMCharacterType::ElectricGuitar, "ElectricGuitar" },
		{ ESMCharacterType::Piano, "Piano" },
		{ ESMCharacterType::Bass, "Bass" }
	};
	FString* TargetPlayerType = PlayerTypeMap.Find(PlayerScoreData[TargetPlayer].CharacterType);

	FString TargetPlayerName = PlayerScoreData[TargetPlayer].PlayerName;

	UE_LOG(LogStereoMix, Warning, TEXT("[%s : %s] %s\nTiles: %d, DamageDealt: %f, DamageReceived: %f, Death: %d, Kill: %d, NoiseBreak: %d, TotalScore: %d"),
		**TargetPlayerTeam,
		**TargetPlayerType,
		*TargetPlayerName,
		PlayerScoreData[TargetPlayer].TotalCapturedTiles,
		PlayerScoreData[TargetPlayer].TotalDamageDealt,
		PlayerScoreData[TargetPlayer].TotalDamageReceived,
		PlayerScoreData[TargetPlayer].TotalDeathCount,
		PlayerScoreData[TargetPlayer].TotalKillCount,
		PlayerScoreData[TargetPlayer].TotalNoiseBreakUsage,
		PlayerScoreData[TargetPlayer].TotalScore());
}
