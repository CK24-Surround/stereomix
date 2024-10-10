// Copyright Surround, Inc. All Rights Reserved.


#include "SMAbilitySystemComponent.h"

#include "GameFramework/Character.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "EngineUtils.h"
#include "GameplayCueManager.h"
#include "AttributeSets/SMCharacterAttributeSet.h"
#include "GameplayEffect/SMGameplayEffect_DynamicTag.h"
#include "Utilities/SMLog.h"


void USMAbilitySystemComponent::AddTag(const FGameplayTag& InGameplayTag)
{
	const TSubclassOf<UGameplayEffect> DynamicTagGE = USMGameplayEffect_DynamicTag::StaticClass();
	if (!ensure(DynamicTagGE))
	{
		return;
	}

	const FGameplayEffectSpecHandle GESpecHandle = MakeOutgoingSpec(DynamicTagGE, 1.0f, MakeEffectContext());
	FGameplayEffectSpec* GESpec = GESpecHandle.Data.Get();
	if (!ensure(GESpec))
	{
		return;
	}

	GESpec->DynamicGrantedTags.AddTag(InGameplayTag);
	ApplyGameplayEffectSpecToSelf(*GESpec);
}

void USMAbilitySystemComponent::RemoveTag(const FGameplayTag& InGameplayTag)
{
	const TSubclassOf<UGameplayEffect> DynamicTagGE = USMGameplayEffect_DynamicTag::StaticClass();
	if (!ensure(DynamicTagGE))
	{
		return;
	}

	FGameplayEffectQuery GEQuery = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(FGameplayTagContainer(InGameplayTag));
	GEQuery.EffectDefinition = DynamicTagGE;

	RemoveActiveEffects(GEQuery);
}

void USMAbilitySystemComponent::AddGC(AActor* TargetActor, const FGameplayTag& GameplayCueTag, const FGameplayCueParameters& Parameters) const
{
	const ACharacter* SourceCharacter = Cast<ACharacter>(GetAvatarActor());
	if (!SourceCharacter)
	{
		return;
	}

	if (GetNetMode() == NM_DedicatedServer)
	{
		ServerRequestGC(TargetActor, GameplayCueTag, EGameplayCueEvent::OnActive, Parameters, false);
	}
	else if (SourceCharacter->IsLocallyControlled())
	{
		ServerRequestGC(TargetActor, GameplayCueTag, EGameplayCueEvent::OnActive, Parameters);
		UGameplayCueManager::AddGameplayCue_NonReplicated(TargetActor, GameplayCueTag, Parameters);
	}
	else
	{
		NET_LOG(SourceCharacter, Warning, TEXT("서버나 로컬 컨트롤인 경우 실행되어야합니다. "));
	}
}

void USMAbilitySystemComponent::RemoveGC(AActor* TargetActor, const FGameplayTag& GameplayCueTag, const FGameplayCueParameters& Parameters) const
{
	const ACharacter* SourceCharacter = Cast<ACharacter>(GetAvatarActor());
	if (!SourceCharacter)
	{
		return;
	}

	if (GetNetMode() == NM_DedicatedServer)
	{
		ServerRequestGC(TargetActor, GameplayCueTag, EGameplayCueEvent::Removed, Parameters, false);
	}
	else if (SourceCharacter->IsLocallyControlled())
	{
		ServerRequestGC(TargetActor, GameplayCueTag, EGameplayCueEvent::Removed, Parameters);
		UGameplayCueManager::RemoveGameplayCue_NonReplicated(TargetActor, GameplayCueTag, Parameters);
	}
	else
	{
		NET_LOG(SourceCharacter, Warning, TEXT("서버나 로컬 컨트롤인 경우 실행되어야합니다. "));
	}
}

void USMAbilitySystemComponent::ExecuteGC(AActor* TargetActor, const FGameplayTag& GameplayCueTag, const FGameplayCueParameters& Parameters) const
{
	const ACharacter* SourceCharacter = Cast<ACharacter>(GetAvatarActor());
	if (!SourceCharacter)
	{
		return;
	}

	if (GetNetMode() == NM_DedicatedServer)
	{
		ServerRequestGC(TargetActor, GameplayCueTag, EGameplayCueEvent::Executed, Parameters, false);
	}
	else if (SourceCharacter->IsLocallyControlled())
	{
		ServerRequestGC(TargetActor, GameplayCueTag, EGameplayCueEvent::Executed, Parameters);
		UGameplayCueManager::ExecuteGameplayCue_NonReplicated(TargetActor, GameplayCueTag, Parameters);
	}
	else
	{
		NET_LOG(SourceCharacter, Warning, TEXT("서버나 로컬 컨트롤인 경우 실행되어야합니다. "));
	}
}

const FGameplayAbilitySpec* USMAbilitySystemComponent::FindGASpecFromClass(const TSubclassOf<UGameplayAbility>& InAbilityClass) const
{
	for (const FGameplayAbilitySpec& GASpec : GetActivatableAbilities())
	{
		if (GASpec.Ability && GASpec.Ability.IsA(InAbilityClass))
		{
			return &GASpec;
		}
	}

	return nullptr;
}

void USMAbilitySystemComponent::OnTagUpdated(const FGameplayTag& Tag, bool TagExists)
{
	OnChangedTag.Broadcast(Tag, TagExists);
}

void USMAbilitySystemComponent::ServerRequestGC_Implementation(AActor* TargetActor, const FGameplayTag& GameplayCueTag, EGameplayCueEvent::Type CueEvent, const FGameplayCueParameters& Parameters, bool bExcludeSelf) const
{
	const ACharacter* SourceCharacter = Cast<ACharacter>(GetAvatarActor());
	if (!SourceCharacter)
	{
		return;
	}

	for (const APlayerController* const PC : TActorRange<APlayerController>(GetWorld()))
	{
		if (bExcludeSelf && (PC == SourceCharacter->GetController()))
		{
			continue;
		}

		ACharacter* PendingCharacter = PC->GetCharacter();
		const USMAbilitySystemComponent* PendingASC = Cast<USMAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(PendingCharacter));
		if (!PendingASC)
		{
			continue;
		}

		PendingASC->ClientExecuteGC(TargetActor, GameplayCueTag, CueEvent, Parameters);
	}
}

void USMAbilitySystemComponent::ClientExecuteGC_Implementation(AActor* TargetActor, const FGameplayTag& GameplayCueTag, EGameplayCueEvent::Type CueEvent, const FGameplayCueParameters& Parameters) const
{
	switch (CueEvent)
	{
		case EGameplayCueEvent::OnActive:
		{
			UGameplayCueManager::AddGameplayCue_NonReplicated(TargetActor, GameplayCueTag, Parameters);
			break;
		}
		case EGameplayCueEvent::WhileActive:
		{
			break;
		}
		case EGameplayCueEvent::Executed:
		{
			UGameplayCueManager::ExecuteGameplayCue_NonReplicated(TargetActor, GameplayCueTag, Parameters);
			break;
		}
		case EGameplayCueEvent::Removed:
		{
			UGameplayCueManager::RemoveGameplayCue_NonReplicated(TargetActor, GameplayCueTag, Parameters);
			break;
		}
	}
}
