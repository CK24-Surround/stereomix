// Fill out your copyright notice in the Description page of Project Settings.


#include "StereoMixCharacterAttributeSet.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayEffectExtension.h"
#include "Characters/StereoMixPlayerCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Utilities/StereoMixeLog.h"
#include "Utilities/StereoMixTag.h"

UStereoMixCharacterAttributeSet::UStereoMixCharacterAttributeSet()
{
	Damage = 0.0f;
	PostureGauge = 0.0f;
	MaxPostureGauge = 100.0f;
	MoveSpeed = 0.0f;
	ProjectileAttack = 0.0f;
	ProjectileAttackCooldown = 0.0f;
}

void UStereoMixCharacterAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UStereoMixCharacterAttributeSet, PostureGauge);
	DOREPLIFETIME(UStereoMixCharacterAttributeSet, MaxPostureGauge);
	DOREPLIFETIME(UStereoMixCharacterAttributeSet, MoveSpeed);
	DOREPLIFETIME(UStereoMixCharacterAttributeSet, ProjectileAttack);
	DOREPLIFETIME(UStereoMixCharacterAttributeSet, ProjectileAttackCooldown);
}

void UStereoMixCharacterAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetDamageAttribute())
	{
		NewValue = NewValue > 0.0f ? NewValue : 0.0f;
	}
}

void UStereoMixCharacterAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if (Attribute == GetPostureGaugeAttribute())
	{
		if (GetPostureGauge() >= GetMaxPostureGauge())
		{
			const UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();
			if (ASC)
			{
				UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(ASC->GetAvatarActor(), StereoMixTag::Event::Character::Stun, FGameplayEventData());
			}
		}
	}
}

void UStereoMixCharacterAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
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
			AStereoMixPlayerCharacter* StereoMixCharacter = Cast<AStereoMixPlayerCharacter>(Data.Target.GetAvatarActor());
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
		const float NewPostureGauge = FMath::Clamp(GetPostureGauge() + GetDamage(), 0.0f, GetMaxPostureGauge());
		SetPostureGauge(NewPostureGauge);
		SetDamage(0.0f);
	}
}

void UStereoMixCharacterAttributeSet::OnRep_PostureGauge(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UStereoMixCharacterAttributeSet, PostureGauge, OldValue);
}

void UStereoMixCharacterAttributeSet::OnRep_MaxPostureGauge(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UStereoMixCharacterAttributeSet, MaxPostureGauge, OldValue);
}

void UStereoMixCharacterAttributeSet::OnRep_MoveSpeed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UStereoMixCharacterAttributeSet, MoveSpeed, OldValue);
}

void UStereoMixCharacterAttributeSet::OnRep_ProjectileAttack(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UStereoMixCharacterAttributeSet, ProjectileAttack, OldValue);
}

void UStereoMixCharacterAttributeSet::OnRep_ProjectileAttackCooldown(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UStereoMixCharacterAttributeSet, ProjectileAttackCooldown, OldValue);
}
