// Copyright Studio Surround. All Rights Reserved.


#include "SMScoreFunctionLibrary.h"

#include "GameFramework/GameStateBase.h"
#include "SMAbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "Components/Core/SMScoreManagerComponent.h"


USMScoreManagerComponent* USMScoreFunctionLibrary::GetScoreManager(const UWorld* World)
{
	const AGameStateBase* GameState = World ? World->GetGameState() : nullptr;
	return GameState ? GameState->GetComponentByClass<USMScoreManagerComponent>() : nullptr;
}

TArray<FPlayerScoreData> USMScoreFunctionLibrary::GetTeamScoreData(const UWorld* World, ESMTeam Team)
{
	if (USMScoreManagerComponent* ScoreManagerComponent = GetScoreManager(World))
	{
		return ScoreManagerComponent->GetTeamScoreData(Team);
	}

	return TArray<FPlayerScoreData>();
}

void USMScoreFunctionLibrary::RecordDamage(AActor* Self, const AActor* Attacker, float InDamageAmount)
{
	const USMAbilitySystemComponent* SourceASC = USMAbilitySystemBlueprintLibrary::GetSMAbilitySystemComponent(Self);
	if (!SourceASC->HasMatchingGameplayTag(SMTags::Character::State::Common::Invincible))
	{
		if (USMScoreManagerComponent* ScoreManagerComponent = GetScoreManager(Self->GetWorld()))
		{
			ScoreManagerComponent->AddTotalDamageReceived(Self, InDamageAmount);
			ScoreManagerComponent->AddTotalDamageDealt(Attacker, InDamageAmount);
		}
	}
}

void USMScoreFunctionLibrary::RecordNoiseBreakUsage(const AActor* Self)
{
	if (USMScoreManagerComponent* ScoreManagerComponent = Self ? GetScoreManager(Self->GetWorld()) : nullptr)
	{
		ScoreManagerComponent->AddTotalNoiseBreakUsage(Self);
	}
}

void USMScoreFunctionLibrary::RecordKillDeathCount(const AActor* LastAttacker, const AActor* Self)
{
	if (USMScoreManagerComponent* ScoreManagerComponent = Self ? GetScoreManager(Self->GetWorld()) : nullptr)
	{
		ScoreManagerComponent->AddTotalKillCount(LastAttacker);
		ScoreManagerComponent->AddTotalDeathCount(Self);
	}
}
