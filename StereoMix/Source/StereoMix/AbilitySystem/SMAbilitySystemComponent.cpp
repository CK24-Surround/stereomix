// Copyright Surround, Inc. All Rights Reserved.


#include "SMAbilitySystemComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "EngineUtils.h"
#include "GameplayCueManager.h"
#include "AttributeSets/SMCharacterAttributeSet.h"
#include "Characters/SMPlayerCharacter.h"
#include "GameplayEffect/SMGameplayEffect_DynamicTag.h"
#include "Utilities/SMLog.h"

void USMAbilitySystemComponent::OnTagUpdated(const FGameplayTag& Tag, bool TagExists)
{
	OnChangedTag.Broadcast(Tag, TagExists);
}

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

void USMAbilitySystemComponent::ExecuteGC(AActor* TargetActor, const FGameplayTag& GameplayCueTag, const FGameplayCueParameters& Parameters)
{
	ACharacter* SourceCharacter = Cast<ACharacter>(GetAvatarActor());
	if (!SourceCharacter)
	{
		return;
	}

	if (GetNetMode() == NM_DedicatedServer)
	{
		ServerExecuteGC(TargetActor, GameplayCueTag, Parameters, false);
	}
	else if (SourceCharacter->IsLocallyControlled())
	{
		ServerExecuteGC(TargetActor, GameplayCueTag, Parameters);
		UGameplayCueManager::ExecuteGameplayCue_NonReplicated(TargetActor, GameplayCueTag, Parameters);
	}
	else
	{
		NET_LOG(SourceCharacter, Warning, TEXT("서버나 로컬 컨트롤인 경우 실행되어야합니다. "));
	}
}

void USMAbilitySystemComponent::ServerExecuteGC_Implementation(AActor* TargetActor, const FGameplayTag& GameplayCueTag, const FGameplayCueParameters& Parameters, bool bExcludeSelf)
{
	ACharacter* SourceCharacter = Cast<ACharacter>(GetAvatarActor());
	if (!SourceCharacter)
	{
		return;
	}

	for (const APlayerController* const PC : TActorRange<APlayerController>(GetWorld()))
	{
		if (!bExcludeSelf || (PC == SourceCharacter->GetController()))
		{
			continue;
		}

		ACharacter* PendingCharacter = PC->GetCharacter();
		USMAbilitySystemComponent* PendingASC = Cast<USMAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(PendingCharacter));
		if (!PendingASC)
		{
			continue;
		}

		PendingASC->ClientExecuteGC(TargetActor, GameplayCueTag, Parameters);
	}
}

void USMAbilitySystemComponent::ClientExecuteGC_Implementation(AActor* TargetActor, const FGameplayTag& GameplayCueTag, const FGameplayCueParameters& Parameters)
{
	UGameplayCueManager::ExecuteGameplayCue_NonReplicated(TargetActor, GameplayCueTag, Parameters);
}
