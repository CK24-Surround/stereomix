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

		// 맵에 현재 적용되는 GE가 로컬에서 실행되었었는지 체크합니다. 여러번 중복 적용되는 상황을 막습니다.
		if (!LocalMoveSpeedMultiplyInfiniteMap.Find(MultiplyMoveSpeedGE))
		{
			// 곱해졌을때 추가될 이동 속도를 구합니다. 그리고 맵에 저장합니다.
			float MoveSpeedToAdd = (SourceAttributeSet->GetBaseMoveSpeed() * MoveSpeedMagnitude) - SourceAttributeSet->GetBaseMoveSpeed();
			LocalMoveSpeedMultiplyInfiniteMap.Add(MultiplyMoveSpeedGE, MoveSpeedToAdd);

			SourceMovement->MaxWalkSpeed += MoveSpeedToAdd;
			NET_LOG(SourceCharacter, Warning, TEXT("[예측 적용] 현재 키 개수: %d 현재 이동 속도: %f 추가된 이동 속도: %f"), LocalMoveSpeedMultiplyInfiniteMap.Num(), SourceMovement->MaxWalkSpeed, MoveSpeedToAdd);
		}
	}

	// 실제 적용을 위한 로직입니다.
	if (SourceCharacter->HasAuthority())
	{
		// MoveSpeedMagnitude값을 SetByCaller로 사용합니다. 
		const UGameplayEffect* GEInstance = MultiplyMoveSpeedGE->GetDefaultObject<UGameplayEffect>();
		FGameplayEffectSpec GESpec(GEInstance, MakeEffectContext(), 1.0f);
		GESpec.SetSetByCallerMagnitude(SMTags::Data::MoveSpeed, MoveSpeedMagnitude);
		ApplyGameplayEffectSpecToSelf(GESpec);
	}
}

void USMAbilitySystemComponent::RemoveMoveSpeedMultiply(const TSubclassOf<UGameplayEffect>& RemoveMoveSpeedGE)
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

		// 로컬에서 실행된 적이 있을때만 적용합니다. 중복 적용되거나 적용되지도 않았는데 이동속도가 내려가는 상황을 막습니다.
		if (float* RemoveMoveSpeedMagnitudePtr = LocalMoveSpeedMultiplyInfiniteMap.Find(RemoveMoveSpeedGE))
		{
			float RemoveMoveSpeedMagnitude = *RemoveMoveSpeedMagnitudePtr;

			// 이동속도가 증가되었던 양 만큼 감소시킵니다.
			SourceMovement->MaxWalkSpeed = SourceMovement->MaxWalkSpeed - RemoveMoveSpeedMagnitude;

			// GE가 제거되었으므로 맵에서도 제거해줍니다.
			LocalMoveSpeedMultiplyInfiniteMap.Remove(RemoveMoveSpeedGE);
			NET_LOG(SourceCharacter, Warning, TEXT("[예측 제거] 현재 키 개수: %d 현재 이동 속도: %f 제거된 이동 속도: %f"), LocalMoveSpeedMultiplyInfiniteMap.Num(), SourceMovement->MaxWalkSpeed, RemoveMoveSpeedMagnitude);
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

		UGameplayEffect* NewGE = NewObject<UGameplayEffect>();
		if (!ensureAlways(NewGE))
		{
			return;
		}

		NewGE->DurationPolicy = EGameplayEffectDurationType::Instant;

		URemoveOtherGameplayEffectComponent& RemoveGEComponent = NewGE->AddComponent<URemoveOtherGameplayEffectComponent>();
		RemoveGEComponent.RemoveGameplayEffectQueries.AddUnique(RemoveGEQuery);

		FGameplayEffectSpec GESpec(NewGE, MakeEffectContext(), 1.0f);
		ApplyGameplayEffectSpecToSelf(GESpec);
	}
}
