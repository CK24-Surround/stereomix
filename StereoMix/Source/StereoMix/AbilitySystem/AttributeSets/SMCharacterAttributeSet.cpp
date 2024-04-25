// Fill out your copyright notice in the Description page of Project Settings.


#include "SMCharacterAttributeSet.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayEffectExtension.h"
#include "Characters/SMPlayerCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Utilities/SMLog.h"
#include "AbilitySystem/SMTags.h"

USMCharacterAttributeSet::USMCharacterAttributeSet()
{
	Damage = 0.0f;

	MaxPostureGauge = 100.0f;
	InitPostureGauge(GetMaxPostureGauge());

	MoveSpeed = 0.0f;

	ProjectileAttack = 0.0f;
	ProjectileAttackCooldown = 0.0f;

	FGameplayTagContainer InitInvincibleStateTags;
	InitInvincibleStateTags.AddTag(SMTags::Character::State::Stun);
	InitInvincibleStateTags.AddTag(SMTags::Character::State::Smashing);
	InvincibleStateTags = InitInvincibleStateTags;
}

void USMCharacterAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USMCharacterAttributeSet, PostureGauge);
	DOREPLIFETIME(USMCharacterAttributeSet, MaxPostureGauge);
	DOREPLIFETIME(USMCharacterAttributeSet, MoveSpeed);
	DOREPLIFETIME(USMCharacterAttributeSet, ProjectileAttack);
	DOREPLIFETIME(USMCharacterAttributeSet, ProjectileAttackCooldown);
}

void USMCharacterAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetDamageAttribute())
	{
		NewValue = NewValue > 0.0f ? NewValue : 0.0f;
	}

	if (Attribute == GetHealAttribute())
	{
		NewValue = NewValue > 0.0f ? NewValue : 0.0f;
	}
}

void USMCharacterAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if (Attribute == GetPostureGaugeAttribute())
	{
		if (GetPostureGauge() <= 0.0f)
		{
			const UAbilitySystemComponent* SourceASC = GetOwningAbilitySystemComponent();
			if (ensure(SourceASC))
			{
				UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(SourceASC->GetAvatarActor(), SMTags::Event::Character::Stun, FGameplayEventData());
			}
		}
	}
}

bool USMCharacterAttributeSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	if (!Super::PreGameplayEffectExecute(Data))
	{
		return false;
	}

	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		if (Data.Target.HasAnyMatchingGameplayTags(InvincibleStateTags))
		{
			Data.EvaluatedData.Magnitude = 0.0f;
			return false;
		}
	}

	return true;
}

void USMCharacterAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	const AActor* OwnerActor = Data.Target.GetAvatarActor();
	if (!OwnerActor)
	{
		return;
	}

	if (Data.EvaluatedData.Attribute == GetMoveSpeedAttribute())
	{
		const UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();
		if (ASC)
		{
			ASMPlayerCharacter* StereoMixCharacter = Cast<ASMPlayerCharacter>(Data.Target.GetAvatarActor());
			if (StereoMixCharacter)
			{
				StereoMixCharacter->SetMaxWalkSpeed(GetMoveSpeed());
			}
		}
		else
		{
			NET_LOG(OwnerActor, Warning, TEXT("GAS와 호환되지 않는 액터입니다."));
		}
	}

	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		const float NewPostureGauge = FMath::Clamp(GetPostureGauge() - GetDamage(), 0.0f, GetMaxPostureGauge());
		SetPostureGauge(NewPostureGauge);
		SetDamage(0.0f);
	}

	if (Data.EvaluatedData.Attribute == GetHealAttribute())
	{
		const float NewPostureGauge = FMath::Clamp(GetPostureGauge() + GetHeal(), 0.0f, GetMaxPostureGauge());
		SetPostureGauge(NewPostureGauge);
		SetHeal(0.0f);
	}
}

void USMCharacterAttributeSet::OnRep_PostureGauge(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USMCharacterAttributeSet, PostureGauge, OldValue);
}

void USMCharacterAttributeSet::OnRep_MaxPostureGauge(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USMCharacterAttributeSet, MaxPostureGauge, OldValue);
}

void USMCharacterAttributeSet::OnRep_MoveSpeed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USMCharacterAttributeSet, MoveSpeed, OldValue);
}

void USMCharacterAttributeSet::OnRep_ProjectileAttack(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USMCharacterAttributeSet, ProjectileAttack, OldValue);
}

void USMCharacterAttributeSet::OnRep_ProjectileAttackCooldown(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USMCharacterAttributeSet, ProjectileAttackCooldown, OldValue);
}
