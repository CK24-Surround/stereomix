// Copyright Surround, Inc. All Rights Reserved.


#include "SMAbilitySystemComponent.h"

#include "SMTags.h"
#include "Abilities/GameplayEffect/SMGameplayEffect_DynamicTag.h"
#include "AttributeSets/SMCharacterAttributeSet.h"
#include "Characters/SMPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayEffectComponents/RemoveOtherGameplayEffectComponent.h"
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

void USMAbilitySystemComponent::ApplyMoveSpeedMultiplyInfinite(const TSubclassOf<UGameplayEffect>& MultiplyMoveSpeedGE, float MoveSpeedMagnitude)
{
	ACharacter* SourceCharacter = Cast<ACharacter>(GetAvatarActor());
	if (!ensureAlways(SourceCharacter))
	{
		return;
	}

	// 예측을 위한 로직입니다.
	if (SourceCharacter->IsLocallyControlled())
	{
		UCharacterMovementComponent* SourceMovement = SourceCharacter->GetCharacterMovement();
		if (!ensureAlways(SourceMovement))
		{
			return;
		}

		const USMCharacterAttributeSet* SourceAttributeSet = GetSet<USMCharacterAttributeSet>();
		if (!ensureAlways(SourceAttributeSet))
		{
			return;
		}

		if (!MoveSpeedMultiplyInfiniteLocalPredictMap.FindOrAdd(MultiplyMoveSpeedGE))
		{
			const float MoveSpeedToAdd = (SourceAttributeSet->GetBaseMoveSpeed() * MoveSpeedMagnitude) - SourceAttributeSet->GetBaseMoveSpeed();
			SourceMovement->MaxWalkSpeed = SourceMovement->MaxWalkSpeed + MoveSpeedToAdd;
			MoveSpeedMultiplyInfiniteLocalPredictMap[MultiplyMoveSpeedGE] = true;
		}
	}

	// 실제 적용을 위한 로직입니다.
	if (SourceCharacter->HasAuthority())
	{
		UGameplayEffect* GEInstance = MultiplyMoveSpeedGE->GetDefaultObject<UGameplayEffect>();
		FGameplayEffectSpec GESpec(GEInstance, MakeEffectContext(), 1.0f);
		GESpec.SetSetByCallerMagnitude(SMTags::Data::MoveSpeed, MoveSpeedMagnitude);
		ApplyGameplayEffectSpecToSelf(GESpec);
	}
}

void USMAbilitySystemComponent::RemoveMoveSpeedMultiply(const TSubclassOf<UGameplayEffect>& RemoveMoveSpeedGE, float MoveSpeedMagnitudeToRemove)
{
	ACharacter* SourceCharacter = Cast<ACharacter>(GetAvatarActor());
	if (!ensure(SourceCharacter))
	{
		return;
	}

	// 예측을 위한 로직입니다.
	if (SourceCharacter->IsLocallyControlled())
	{
		UCharacterMovementComponent* SourceMovement = SourceCharacter->GetCharacterMovement();
		if (!ensureAlways(SourceMovement))
		{
			return;
		}

		const USMCharacterAttributeSet* SourceAttributeSet = GetSet<USMCharacterAttributeSet>();
		if (!ensureAlways(SourceAttributeSet))
		{
			return;
		}

		if (MoveSpeedMultiplyInfiniteLocalPredictMap.FindOrAdd(RemoveMoveSpeedGE))
		{
			const float MoveSpeedToAdd = (SourceAttributeSet->GetBaseMoveSpeed() * MoveSpeedMagnitudeToRemove) - SourceAttributeSet->GetBaseMoveSpeed();
			SourceMovement->MaxWalkSpeed = SourceMovement->MaxWalkSpeed - MoveSpeedToAdd;
			MoveSpeedMultiplyInfiniteLocalPredictMap[RemoveMoveSpeedGE] = false;
		}
	}

	// 실제 적용을 위한 로직입니다.
	if (SourceCharacter->HasAuthority())
	{
		FGameplayEffectQuery RemoveGEQuery;
		RemoveGEQuery.EffectDefinition = RemoveMoveSpeedGE;
		if (GetActiveEffects(RemoveGEQuery).IsEmpty())
		{
			return;
		}

		UGameplayEffect* NewGE = UGameplayEffect::StaticClass()->GetDefaultObject<UGameplayEffect>();
		NewGE->DurationPolicy = EGameplayEffectDurationType::Instant;

		URemoveOtherGameplayEffectComponent& RemoveGEComponent = NewGE->AddComponent<URemoveOtherGameplayEffectComponent>();
		RemoveGEComponent.RemoveGameplayEffectQueries.AddUnique(RemoveGEQuery);

		FGameplayEffectSpec GESpec(NewGE, MakeEffectContext(), 1.0f);
		ApplyGameplayEffectSpecToSelf(GESpec);
	}
}
