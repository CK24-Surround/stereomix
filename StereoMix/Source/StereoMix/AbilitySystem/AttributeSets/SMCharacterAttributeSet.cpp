// Copyright Surround, Inc. All Rights Reserved.


#include "SMCharacterAttributeSet.h"

#include "AbilitySystem/SMTags.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"
#include "Utilities/SMLog.h"

USMCharacterAttributeSet::USMCharacterAttributeSet()
{
	Damage = 0.0f;

	MaxPostureGauge = 100.0f;
	InitPostureGauge(GetMaxPostureGauge());

	MoveSpeed = 0.0f;

	InitMaxStamina(100.0f);
	InitStamina(GetMaxStamina());

	InitMaxSkillGauge(100.0f);
	InitSkillGauge(GetMaxSkillGauge());

	InvincibleStateTags.AddTag(SMTags::Character::State::Common::Invincible);
	InvincibleStateTags.AddTag(SMTags::Character::State::Common::Neutralized);
	InvincibleStateTags.AddTag(SMTags::Character::State::Common::Immune);
	InvincibleStateTags.AddTag(SMTags::Character::State::Common::NoiseBreak);
	InvincibleStateTags.AddTag(SMTags::Character::State::Common::NoiseBreaked);
	InvincibleStateTags.AddTag(SMTags::Character::State::Bass::Charge);

	UnlimitStaminaTags.AddTag(SMTags::Character::State::Common::UnlimitStamina);
}

void USMCharacterAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USMCharacterAttributeSet, PostureGauge);
	DOREPLIFETIME(USMCharacterAttributeSet, MaxPostureGauge);
	DOREPLIFETIME(USMCharacterAttributeSet, MoveSpeed);
	DOREPLIFETIME(USMCharacterAttributeSet, Stamina);
	DOREPLIFETIME(USMCharacterAttributeSet, MaxStamina);
	DOREPLIFETIME(USMCharacterAttributeSet, SkillGauge);
	DOREPLIFETIME(USMCharacterAttributeSet, MaxSkillGauge);
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

	if (Attribute == GetHealSkillGaugeAttribute())
	{
		NewValue = NewValue > 0.0f ? NewValue : 0.0f;
	}

	if (Attribute == GetHealStaminaAttribute())
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
				UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(SourceASC->GetAvatarActor(), SMTags::Event::Character::Neutralize, FGameplayEventData());
			}
		}
	}

	if (Attribute == GetStaminaAttribute())
	{
		if (GetStamina() <= 0.0f)
		{
			if (UAbilitySystemComponent* SourceASC = GetOwningAbilitySystemComponent())
			{
				UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(SourceASC->GetAvatarActor(), SMTags::Event::Character::Exhausted, FGameplayEventData());
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

	if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
	{
		if (Data.Target.HasAnyMatchingGameplayTags(UnlimitStaminaTags))
		{
			Data.EvaluatedData.Magnitude = Data.EvaluatedData.Magnitude < 0.0f ? 0.0f : Data.EvaluatedData.Magnitude;
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

	if (Data.EvaluatedData.Attribute == GetHealSkillGaugeAttribute())
	{
		const float NewSkillGauge = FMath::Clamp(GetSkillGauge() + GetHealSkillGauge(), 0.0f, GetMaxSkillGauge());
		SetSkillGauge(NewSkillGauge);
		SetHealSkillGauge(0.0f);
	}

	if (Data.EvaluatedData.Attribute == GetHealStaminaAttribute())
	{
		const float NewStamina = FMath::Clamp(GetStamina() + GetHealStamina(), 0.0f, GetMaxStamina());
		SetStamina(NewStamina);
		SetHealStamina(0.0f);
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

void USMCharacterAttributeSet::OnRep_Stamina(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USMCharacterAttributeSet, Stamina, OldValue);
}

void USMCharacterAttributeSet::OnRep_MaxStamina(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USMCharacterAttributeSet, MaxStamina, OldValue);
}

void USMCharacterAttributeSet::OnRep_SkillGauge(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USMCharacterAttributeSet, SkillGauge, OldValue);
}

void USMCharacterAttributeSet::OnRep_MaxSkillGauge(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USMCharacterAttributeSet, MaxSkillGauge, OldValue);
}
